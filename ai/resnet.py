import pathlib

import lightning.pytorch as pl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sklearn.metrics
import torch.nn as nn
import torch.optim
import torchvision.io
import torchvision.transforms as transforms
import tqdm
from lightning.pytorch.callbacks import LearningRateMonitor, ModelCheckpoint
from torch.utils.data import DataLoader, Dataset
from torchvision import transforms

labels = [
    "auger_bit_24_400",
    "auger_drill_bit_20_450",
    "chain_saw_blade_f_250",
    "self_feeding_bit_50",
    "spade_drill_bit_35",
    "auger_drill_bit_20_235",
    "auger_drill_bit_30_400",
    "circular_saw_blade_makita_190",
    "self_feeding_drill_bit_30_90",
    "twist_drill_bit_32_90",
    "auger_drill_bit_20_400",
    "brad_point_drill_bit_20_150",
    "saber_saw_blade",
    "spade_drill_bit_25",
]


class ResNetBlock(nn.Module):
    def __init__(self, c_in, act_fn, subsample=False, c_out=-1):
        super().__init__()
        if not subsample:
            c_out = c_in
            stride = 1
        else:
            stride = 2

        self.net = nn.Sequential(
            nn.Conv2d(c_in, c_out, kernel_size=3, padding=1, stride=stride, bias=False),
            nn.BatchNorm2d(c_out),
            act_fn(),
            nn.Conv2d(c_out, c_out, kernel_size=3, padding=1, bias=False),
            nn.BatchNorm2d(c_out),
        )

        if subsample:
            self.downsample = nn.Conv2d(c_in, c_out, kernel_size=1, stride=2)
        else:
            self.downsample = None
        self.act_fn = act_fn()

    def forward(self, x):
        z = self.net(x)
        if self.downsample is not None:
            x = self.downsample(x)
        out = z + x
        out = self.act_fn(out)
        return out


class ResNet(nn.Module):
    def __init__(
        self,
        num_classes=14,
        num_blocks=[3, 3, 3],
        c_hidden=[16, 32, 64],
        act_fn=nn.ReLU,
        **kwargs,
    ):
        super().__init__()
        self.input_net = nn.Sequential(
            nn.Conv2d(3, c_hidden[0], kernel_size=3, padding=1, bias=False),
            nn.BatchNorm2d(c_hidden[0]),
            act_fn(),
        )
        blocks = []
        for block_idx, block_count in enumerate(num_blocks):
            for bc in range(block_count):
                if bc == 0 and block_idx > 0:
                    subsample = True
                    c_in = c_hidden[block_idx - 1]
                else:
                    subsample = False
                    c_in = c_hidden[block_idx]
                block = ResNetBlock(
                    c_in=c_in,
                    act_fn=act_fn,
                    subsample=subsample,
                    c_out=c_hidden[block_idx],
                )
                blocks.append(block)
        self.blocks = nn.Sequential(*blocks)

        self.classification_head = nn.Sequential(
            nn.AdaptiveAvgPool2d((1, 1)),
            nn.Flatten(),
            nn.Linear(c_hidden[-1], num_classes),
        )

        # initialise weights
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode="fan_out", nonlinearity="relu")
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def forward(self, x):
        x = self.input_net(x)
        x = self.blocks(x)
        x = self.classification_head(x)
        return x


class LitClassifier(pl.LightningModule):
    def __init__(self, network):
        super().__init__()
        self.net = network
        self.loss_function = nn.CrossEntropyLoss()
        self.acc_function = lambda y_hat, y: (y_hat.argmax(dim=-1) == y).float().mean()

    def forward(self, x):
        return self.net(x)

    def training_step(self, batch, batch_idx):
        x, y = batch
        y_hat = self.net(x)
        loss = self.loss_function(y_hat, y)
        acc = self.acc_function(y_hat, y)
        self.log("train_loss", loss)
        self.log("train_acc", acc)
        return loss

    def validation_step(self, batch, batch_idx):
        x, y = batch
        y_hat = self.net(x)
        loss = self.loss_function(y_hat, y)
        acc = self.acc_function(y_hat, y)
        self.log("val_loss", loss)
        self.log("val_acc", acc)
        # plot_confusion_matrix(y, y_hat)
        return loss

    def test_step(self, batch, batch_idx):
        x, y = batch
        y_hat = self.net(x)
        loss = self.loss_function(y_hat, y)
        acc = self.acc_function(y_hat, y)
        self.log("test_loss", loss)
        self.log("test_acc", acc)

    def configure_optimizers(self):
        optimizer = torch.optim.SGD(
            self.parameters(), lr=0.1, momentum=0.9, weight_decay=1e-4
        )
        scheduler = torch.optim.lr_scheduler.MultiStepLR(
            optimizer, milestones=[100, 150], gamma=0.1
        )
        return [optimizer], [scheduler]


class ToolDataset(Dataset):
    def __init__(self, img_dir, transform=None, target_transform=None):
        self.img_dir = img_dir
        self.img_paths = list(img_dir.glob("*.png"))
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.img_paths)

    def __getitem__(self, idx):
        img_path = self.img_paths[idx]
        image = torchvision.io.read_image(str(img_path))
        image = image.float() / 255
        label = img_path.stem.split("__")[0]
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
            label = self.target_transform(label)
        return image, label


def label_transform(label):
    return labels.index(label)


def plot_confusion_matrix(y, y_hat):
    y = y.cpu()
    y_hat = y_hat.cpu()
    y_hat = np.argmax(y_hat, axis=1)
    confusion_matrix = sklearn.metrics.confusion_matrix(y.cpu(), y_hat.cpu())
    confusion_matrix = confusion_matrix / np.sum(confusion_matrix, axis=1)[:, None]
    print(np.unique(y))
    print(np.unique(y_hat))
    print(len(np.array(labels)[np.unique(y)]))
    disp = sklearn.metrics.ConfusionMatrixDisplay(
        confusion_matrix=confusion_matrix, display_labels=np.array(labels)[np.unique(y)]
    )
    disp.plot()
    output_folder = pathlib.Path("confusion_matrices")
    idx = len(list(output_folder.glob("*.pdf")))
    plt.savefig(output_folder / f"matrix_{idx}.pdf")
    plt.close()


if __name__ == "__main__":
    torch.set_float32_matmul_precision("high")

    img_dir = pathlib.Path("/data/ENAC/iBOIS/images")

    # Determine the mean and std for the training data
    train_dataset = ToolDataset(
        img_dir / "train",
        transform=lambda x: x.float(),
        target_transform=label_transform,
    )
    train_imgs = []
    n_train_imgs = len(train_dataset)
    print("Loading train images to compute mean and std")
    for i in tqdm.tqdm(np.arange(n_train_imgs)):
        train_imgs.append(train_dataset[i][0])
    train_imgs = torch.stack(train_imgs)
    train_means = train_imgs.mean(axis=(0, 2, 3))
    train_stds = train_imgs.std(axis=(0, 2, 3))
    print(train_means)
    print(train_stds)
    quit()
    df = pd.DataFrame({"Mean": train_means, "STD": train_stds})
    df.to_csv("train_means_stds.csv")

    # Create train and validation datasets
    image_transform = transforms.Normalize(train_means, train_stds)
    train_dataset = ToolDataset(
        img_dir / "train", transform=image_transform, target_transform=label_transform
    )
    val_dataset = ToolDataset(
        img_dir / "val", transform=image_transform, target_transform=label_transform
    )

    # Create data loaders
    train_loader = DataLoader(train_dataset, batch_size=25, shuffle=True, num_workers=8)
    val_loader = DataLoader(val_dataset, batch_size=25, num_workers=8)

    # Create instance of lightning module
    classifier = LitClassifier(ResNet())

    # Train
    trainer = pl.Trainer(
        max_epochs=201,
        callbacks=[
            ModelCheckpoint(save_weights_only=True, mode="max", monitor="val_acc"),
            LearningRateMonitor("epoch"),
        ],
    )
    trainer.logger._log_graph = (True,)
    trainer.fit(
        model=classifier,
        train_dataloaders=train_loader,
        val_dataloaders=val_loader,
        ckpt_path="lightning_logs/version_4/checkpoints/epoch=199-step=22800.ckpt",
    )
