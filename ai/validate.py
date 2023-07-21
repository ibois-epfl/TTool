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

# model_type = "ResNet"
# model_type = "ResNet222"
# model_type = "TransferResNet"
# model_type = "TransferEfficientNet"
# model_type = "TransferEfficientNetNewSplit"
model_type = "TransferEfficientNetAugmentationTwoDataSets"

if model_type == "ResNet":
    train_means_stds = pd.read_csv("train_means_stds.csv")
    train_means = train_means_stds["Mean"].values
    train_stds = train_means_stds["STD"].values
    image_transform = transforms.Normalize(train_means, train_stds)
    image_transform = transforms.Normalize(train_means, train_stds)
    ckpt = "./lightning_logs/version_0/checkpoints/epoch=199-step=22800.ckpt"
    network = models.ResNet()
if model_type == "ResNet222":
    train_means_stds = pd.read_csv("train_means_stds.csv")
    train_means = train_means_stds["Mean"].values
    train_stds = train_means_stds["STD"].values
    image_transform = transforms.Normalize(train_means, train_stds)
    image_transform = transforms.Normalize(train_means, train_stds)
    ckpt = "./lightning_logs/version_1/checkpoints/epoch=184-step=21090.ckpt"
    network = models.ResNet(nun_blocks=[2, 2, 2])
elif model_type == "TransferResNet":
    image_transform = torchvision.models.ResNet18_Weights.DEFAULT.transforms()
    ckpt = "./lightning_logs/version_2/checkpoints/epoch=88-step=10146.ckpt"
    network = models.TransferResNet()
elif model_type == "TransferEfficientNet":
    image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
    ckpt = "./lightning_logs/version_3/checkpoints/epoch=176-step=20178.ckpt"
    network = models.TransferEfficientNet()
elif model_type == "TransferEfficientNetNewSplit":
    image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
    ckpt = "./lightning_logs/version_61/checkpoints/epoch=45-step=28934.ckpt"
    network = models.TransferEfficientNet()
elif model_type == "TransferEfficientNetAugmentationTwoDataSets":
    image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
    ckpt = "./lightning_logs/version_148/checkpoints/epoch=19-step=3840.ckpt"
    network = models.TransferEfficientNet(num_classes=len(datasets.labels))

# img_dir = pathlib.Path("/data/ENAC/iBOIS/images")
# val_dataset = datasets.ToolDataset(
#     img_dir / "val",
#     transform=image_transform,
#     target_transform=datasets.label_transform,
# )
# data_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_images/")
# val_dataset = datasets.FabricationDataset(
#     data_dir,
#     transform=image_transform,
#     target_transform=datasets.label_transform,
#     subsampling=100,
# )
img_dir = pathlib.Path("/data/ENAC/iBOIS/test_dataset/images/val")
val_dataset = datasets.ToolDataset(
    img_dir,
    transform=image_transform,
    target_transform=datasets.label_transform,
)
val_dataloader = DataLoader(val_dataset, batch_size=25, num_workers=8)

model = train.LitClassifier.load_from_checkpoint(
    ckpt,
    network=network,
)
model.eval()

trainer = pl.Trainer(accelerator="gpu", logger=None)
val_result = trainer.test(model, dataloaders=val_dataloader, verbose=True)
res = trainer.predict(model, dataloaders=val_dataloader)
y_hats = [i[0] for i in res]
ys = [i[1] for i in res]
y_hat = torch.cat(y_hats)
y = torch.cat(ys)

acc = (y == y_hat).float().mean()

confusion_matrix = sklearn.metrics.confusion_matrix(
    y, y_hat, labels=np.argsort(datasets.labels), normalize="true"
)
disp = sklearn.metrics.ConfusionMatrixDisplay(
    confusion_matrix=confusion_matrix,
    display_labels=np.sort(np.array(datasets.labels)),  # [np.unique(y)]),
)
disp.plot(text_kw={"fontsize": 6})
ax = plt.gca()
ax.tick_params(axis="both", which="major", labelsize=6)
ax.tick_params(axis="both", which="minor", labelsize=6)
plt.xticks(rotation=90)
plt.tight_layout()
plt.title(f"{model_type} acc={acc:.2f}")
plt.savefig(f"confusion_matrix_{model_type}_test_dataset.pdf")
plt.close()
