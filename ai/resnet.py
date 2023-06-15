import pathlib

import lightning.pytorch as pl
import torch.nn as nn
import torch.optim
import torchvision.io
from torch.utils.data import DataLoader, Dataset


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

    def training_step(self, batch, batch_idx):
        x, y = batch
        y_hat = self.net(x)
        loss = nn.functional.cross_entropy(y_hat, y)
        self.log("train_loss", loss)
        return loss

    def configure_optimizers(self):
        optimizer = torch.optim.SGD(self.parameters(), lr=1e-3)
        return optimizer


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
        label = img_path.stem.split("__")[0]
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
            label = self.target_transform(label)
        return image, label


def label_transform(label):
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
    return labels.index(label)


def image_transform(img):
    return img.float()


if __name__ == "__main__":
    img_dir = pathlib.Path(
        "/home/aymanns/projects/ENAC_augmented_carpentry/data/images"
    )
    dataset = ToolDataset(
        img_dir, transform=image_transform, target_transform=label_transform
    )
    train_loader = DataLoader(dataset, batch_size=1, shuffle=True, num_workers=8)
    classifier = LitClassifier(ResNet())

    trainer = pl.Trainer(limit_train_batches=100, max_epochs=10)
    trainer.fit(model=classifier, train_dataloaders=train_loader)
