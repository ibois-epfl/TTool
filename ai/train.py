"""
Script for training a network using pytorch lightning.
"""

import pathlib

import lightning.pytorch as pl
import numpy as np
import pandas as pd
import torch.nn as nn
import torch.optim
import torch.utils.data
import torchvision.io
import torchvision.transforms as transforms
import tqdm
from lightning.pytorch.callbacks import LearningRateMonitor, ModelCheckpoint
from torchvision import transforms

import datasets
import models


class LitClassifier(pl.LightningModule):
    def __init__(self, network):
        """
        Lightning module used to "interact" with the network.
        """
        super().__init__()
        self.net = network
        self.loss_function = nn.CrossEntropyLoss()
        self.acc_function = lambda y_hat, y: (y_hat.argmax(dim=-1) == y).float().mean()

    def forward(self, x):
        return self.net(x)

    def training_step(self, batch, batch_idx):
        """
        Function exectued for each step during training.
        """
        x, y = batch
        y_hat = self.net(x)

        # Calculate and log loss as well as accuracy
        loss = self.loss_function(y_hat, y)
        acc = self.acc_function(y_hat, y)
        self.log("train_loss", loss)
        self.log("train_acc", acc)
        return loss

    def validation_step(self, batch, batch_idx):
        """
        Function exectued for each step during validation.
        """
        x, y = batch
        y_hat = self.net(x)

        # Calculate and log loss as well as accuracy
        loss = self.loss_function(y_hat, y)
        acc = self.acc_function(y_hat, y)
        self.log("val_loss", loss)
        self.log("val_acc", acc)
        return loss

    def test_step(self, batch, batch_idx):
        """
        Function exectued for each step during testing.
        """
        x, y = batch
        y_hat = self.net(x)
        # Calculate and log loss as well as accuracy
        loss = self.loss_function(y_hat, y)
        acc = self.acc_function(y_hat, y)
        self.log("test_loss", loss)
        self.log("test_acc", acc)

    def predict_step(self, batch, batch_idx):
        """
        Function exectued when predicting.
        """
        x, y = batch
        y_hat = self.net(x).argmax(dim=-1)
        return y_hat, y

    def configure_optimizers(self):
        # Use stochatic gradient descent
        optimizer = torch.optim.SGD(
            self.parameters(), lr=0.1, momentum=0.9, weight_decay=1e-4
        )
        # Decrease learning rate over time
        scheduler = torch.optim.lr_scheduler.MultiStepLR(
            optimizer, milestones=[15, 30], gamma=0.1
        )
        return [optimizer], [scheduler]


if __name__ == "__main__":
    torch.set_float32_matmul_precision("high")

    # Select one of the following model types: "ResNet", "TransferResNet", "TransferEfficientNet"
    MODEL_TYPE = "TransferEfficientNet"

    if MODEL_TYPE == "ResNet":
        # Determine the mean and std for the training data
        # in order to z-score the images.

        img_dir = pathlib.Path("/data/ENAC/iBOIS/images")
        train_dataset = datasets.ToolDataset(
            img_dir / "train",
            transform=lambda x: x.float(),
            target_transform=datasets.label_transform,
        )
        train_imgs = []
        n_train_imgs = len(train_dataset)
        print("Loading train images to compute mean and std")
        for i in tqdm.tqdm(np.arange(n_train_imgs)):
            train_imgs.append(train_dataset[i][0])
        train_imgs = torch.stack(train_imgs)
        train_means = train_imgs.mean(axis=(0, 2, 3))
        train_stds = train_imgs.std(axis=(0, 2, 3))
        df = pd.DataFrame({"Mean": train_means, "STD": train_stds})
        df.to_csv("train_means_stds.csv")

        normalization_transform = transforms.Normalize(train_means, train_stds)

        # Get ResNet instance
        network = models.ResNet(num_blocks=[2, 2, 2], num_classes=len(datasets.labels))

    elif MODEL_TYPE == "TransferResNet":
        # In order for the transfer learning to work properly,
        # we  have to use the same normalization used during
        # the training of the initial weights.
        normalization_transform = (
            torchvision.models.ResNet18_Weights.DEFAULT.transforms()
        )
        network = models.TransferResNet(num_classes=len(datasets.labels))
    elif MODEL_TYPE == "TransferEfficientNet":
        normalization_transform = (
            torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
        )
        network = models.TransferEfficientNet(num_classes=len(datasets.labels))

    # Create train and validation datasets

    transform = torchvision.transforms.Compose(
        [datasets.augmentation_transforms, normalization_transform]
    )

    # First data set (hand held by Naravich)
    img_dir = pathlib.Path("/data/ENAC/iBOIS/images")
    tool_train_dataset = datasets.ToolDataset(
        img_dir / "train",
        transform=transform,
        target_transform=datasets.label_transform,
        subsampling=5,
    )
    tool_val_dataset = datasets.ToolDataset(
        img_dir / "val",
        transform=normalization_transform,
        target_transform=datasets.label_transform,
        # subsampling=20,
    )

    # Data set using the labled fabrication videos
    img_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_images")
    fabrication_train_dataset = datasets.FabricationDataset(
        img_dir,
        transform=transform,
        target_transform=datasets.label_transform,
        # videos=[1, 2, 4, 20],
        videos=[1, 3, 4, 5, 15, 16, 17, 18, 19],
        # subsampling=10,
        # subsampling=50,
        subsampling=3,
    )
    fabrication_val_dataset = datasets.FabricationDataset(
        img_dir,
        transform=normalization_transform,
        target_transform=datasets.label_transform,
        # videos=[3, 18],
        videos=[2, 20],
        # subsampling=20,
    )

    # Data set with tools attached acquired by Naravich
    img_dir = pathlib.Path("/data/ENAC/iBOIS/test_dataset/images")
    attached_train_dataset = datasets.ToolDataset(
        img_dir / "train",
        transform=transform,
        target_transform=datasets.label_transform,
        subsampling=40,
    )
    attached_val_dataset = datasets.ToolDataset(
        img_dir / "val",
        transform=normalization_transform,
        target_transform=datasets.label_transform,
    )

    # Synthetic data set
    synthetic_train_dataset = datasets.SyntheticDataset(
        pathlib.Path("/data/ENAC/iBOIS/ToolheadSyntheticDataset"),
        transform=transform,
        target_transform=datasets.label_transform,
        subdir="train",
    )
    synthetic_val_dataset = datasets.SyntheticDataset(
        pathlib.Path("/data/ENAC/iBOIS/ToolheadSyntheticDataset"),
        transform=normalization_transform,
        target_transform=datasets.label_transform,
        subdir="val",
    )

    # Select what data to use for training and validation
    # train_dataset = torch.utils.data.ConcatDataset(
    #     [tool_train_dataset, fabrication_train_dataset, attached_train_dataset]
    # )
    # train_dataset = attached_train_dataset
    train_dataset = torch.utils.data.ConcatDataset(
        [synthetic_train_dataset, attached_train_dataset]
    )
    val_dataset = torch.utils.data.ConcatDataset(
        [tool_val_dataset, fabrication_val_dataset, attached_val_dataset]
    )

    # Create data loaders
    train_loader = torch.utils.data.DataLoader(
        train_dataset, batch_size=90, shuffle=True, num_workers=48
    )
    val_loader = torch.utils.data.DataLoader(val_dataset, batch_size=90, num_workers=48)

    # Create instance of lightning module
    classifier = LitClassifier(network)

    # Train
    trainer = pl.Trainer(
        max_epochs=45,
        callbacks=[
            ModelCheckpoint(mode="max", monitor="val_acc"),
            LearningRateMonitor("epoch"),
        ],
        default_root_dir="/data/ENAC/iBOIS/lightning_logs",
    )
    trainer.logger._log_graph = (True,)
    trainer.fit(
        model=classifier,
        train_dataloaders=train_loader,
        val_dataloaders=val_loader,
    )
