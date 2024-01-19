"""
Script for validating the model and computing the confusion matrix.
"""

import pathlib

import lightning.pytorch as pl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sklearn.metrics
import torch
import torchvision
from torch.utils.data import DataLoader
from torchvision import transforms

import datasets
import models
import train

torch.set_float32_matmul_precision("high")

# Select one of the following model types:
# "ResNet"
# "ResNet222"
# "TransferResNet"
# "TransferEfficientNet"
# "TransferEfficientNetNewSplit"
# "TransferEfficientNetAugmentationTwoDataSets"
model_type = "TransferEfficientNetAugmentationTwoDataSets"

# Get the correct normalization transform, checkpoint and network based on the model type
if model_type == "ResNet":
    train_means_stds = pd.read_csv("train_means_stds.csv")
    train_means = train_means_stds["Mean"].values
    train_stds = train_means_stds["STD"].values
    image_transform = transforms.Normalize(train_means, train_stds)
    image_transform = transforms.Normalize(train_means, train_stds)
    ckpt = "/data/ENAC/iBOIS/lightning_logs/version_0/checkpoints/epoch=199-step=22800.ckpt"
    network = models.ResNet()
if model_type == "ResNet222":
    train_means_stds = pd.read_csv("train_means_stds.csv")
    train_means = train_means_stds["Mean"].values
    train_stds = train_means_stds["STD"].values
    image_transform = transforms.Normalize(train_means, train_stds)
    image_transform = transforms.Normalize(train_means, train_stds)
    ckpt = "/data/ENAC/iBOIS/lightning_logs/version_1/checkpoints/epoch=184-step=21090.ckpt"
    network = models.ResNet(nun_blocks=[2, 2, 2])
elif model_type == "TransferResNet":
    image_transform = torchvision.models.ResNet18_Weights.DEFAULT.transforms()
    ckpt = (
        "/data/ENAC/iBOIS/lightning_logs/version_2/checkpoints/epoch=88-step=10146.ckpt"
    )
    network = models.TransferResNet()
elif model_type == "TransferEfficientNet":
    image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
    ckpt = "/data/ENAC/iBOIS/lightning_logs/version_3/checkpoints/epoch=176-step=20178.ckpt"
    network = models.TransferEfficientNet()
elif model_type == "TransferEfficientNetNewSplit":
    image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
    ckpt = "/data/ENAC/iBOIS/lightning_logs/version_61/checkpoints/epoch=45-step=28934.ckpt"
    network = models.TransferEfficientNet()
elif model_type == "TransferEfficientNetAugmentationTwoDataSets":
    image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()

    # ckpt = "/data/ENAC/iBOIS/lightning_logs/version_148/checkpoints/epoch=19-step=3840.ckpt"

    # Extra augmentation
    # ckpt = "/data/ENAC/iBOIS/lightning_logs/version_181/checkpoints/epoch=41-step=8064.ckpt"

    # Trained on synthetic data only
    # ckpt = "/data/ENAC/iBOIS/lightning_logs/version_187/checkpoints/epoch=8-step=900.ckpt"

    # trained on synthetic + test_train dataset
    # ckpt = "/data/ENAC/iBOIS/lightning_logs/lightning_logs/version_0/checkpoints/epoch=33-step=5270.ckpt"

    # trained on test_train dataset
    ckpt = "/data/ENAC/iBOIS/lightning_logs/lightning_logs/version_1/checkpoints/epoch=41-step=2520.ckpt"

    # trained on synthetic + test_train dataset subsampled by a factor of 40
    # ckpt = "/data/ENAC/iBOIS/lightning_logs/lightning_logs/version_3/checkpoints/epoch=32-step=3201.ckpt"

    # trained on test_train dataset subsampled by a factor of 40
    # ckpt = "/data/ENAC/iBOIS/lightning_logs/lightning_logs/version_9/checkpoints/epoch=44-step=90.ckpt"

    network = models.TransferEfficientNet(num_classes=len(datasets.labels))

# First data set (hand held by Naravich)
img_dir = pathlib.Path("/data/ENAC/iBOIS/images")
tool_val_dataset = datasets.ToolDataset(
    img_dir / "val",
    transform=image_transform,
    target_transform=datasets.label_transform,
)

# Data set using the labeled fabrication videos
data_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_images/")
fabrication_val_dataset = datasets.FabricationDataset(
    data_dir,
    transform=image_transform,
    target_transform=datasets.label_transform,
    subsampling=100,
)

# Data set with tool attached
img_dir = pathlib.Path("/data/ENAC/iBOIS/test_dataset/images/val")
attached_val_dataset = datasets.ToolDataset(
    img_dir,
    transform=image_transform,
    target_transform=datasets.label_transform,
)

# Demo data sets
data_dir = pathlib.Path("/data/ENAC/iBOIS/toolhead_demo")
demo1_val_dataset = datasets.ToolheadDemoDataset(
    data_dir,
    transform=image_transform,
    target_transform=datasets.label_transform,
)
data_dir = pathlib.Path("/data/ENAC/iBOIS/toolhead_demo2")
demo2_val_dataset = datasets.ToolheadDemoDataset(
    data_dir,
    transform=image_transform,
    target_transform=datasets.label_transform,
)
demo_val_dataset = torch.utils.data.ConcatDataset(
    [demo1_val_dataset, demo2_val_dataset]
)

# Synthetic data set
synthetic_val_dataset = datasets.SyntheticDataset(
    pathlib.Path("/data/ENAC/iBOIS/ToolheadSyntheticDataset"),
    transform=image_transform,
    target_transform=datasets.label_transform,
    subdir="val",
)

val_dataset = demo_val_dataset
val_dataloader = DataLoader(val_dataset, batch_size=25, num_workers=8)

model = train.LitClassifier.load_from_checkpoint(
    ckpt,
    network=network,
)
model.eval()

trainer = pl.Trainer(
    accelerator="gpu", logger=None, default_root_dir="/data/ENAC/iBOIS/lightning_logs"
)
val_result = trainer.test(model, dataloaders=val_dataloader, verbose=True)

# Do prediction and split results into ground truth and prediction
res = trainer.predict(model, dataloaders=val_dataloader)
y_hats = [i[0] for i in res]
ys = [i[1] for i in res]
y_hat = torch.cat(y_hats)
y = torch.cat(ys)

# Compute accuracy
acc = (y == y_hat).float().mean()

# Compute confusion matrix
confusion_matrix = sklearn.metrics.confusion_matrix(
    y, y_hat, labels=np.argsort(datasets.labels), normalize="true"
)

# Plot confusion matrix
disp = sklearn.metrics.ConfusionMatrixDisplay(
    confusion_matrix=confusion_matrix,
    display_labels=np.sort(np.array(datasets.labels)),  # [np.unique(y)]),
)
disp.plot(text_kw={"fontsize": 6})
ax = plt.gca()
ax.tick_params(axis="both", which="major", labelsize=6)
ax.tick_params(axis="both", which="minor", labelsize=6)
plt.xticks(rotation=90)
# plt.title(f"{model_type} acc={acc:.2f}")
plt.title(f"acc={acc:.2f}")
plt.tight_layout()
plt.savefig(
    f"confusion_matrix_{model_type}_trained_on_test_train_dataset_validated_on_test_dataset.pdf"
)
plt.close()
