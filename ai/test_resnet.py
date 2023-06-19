import pathlib

import lightning.pytorch as pl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sklearn
import torch
from torch.utils.data import DataLoader
from torchvision import transforms

from resnet import LitClassifier, ResNet, ToolDataset, label_transform, labels

torch.set_float32_matmul_precision("high")

img_dir = pathlib.Path("/data/ENAC/iBOIS/images")
train_means_stds = pd.read_csv("train_means_stds.csv")
train_means = train_means_stds["Mean"].values
train_stds = train_means_stds["STD"].values
image_transform = transforms.Normalize(train_means, train_stds)
image_transform = transforms.Normalize(train_means, train_stds)
test_dataset = ToolDataset(
    img_dir / "test", transform=image_transform, target_transform=label_transform
)
test_dataloader = DataLoader(test_dataset, batch_size=25, num_workers=8)

model = LitClassifier.load_from_checkpoint(
    "./lightning_logs/version_4/checkpoints/bck_epoch=199-step=22800.ckpt",
    network=ResNet(),
)
model.eval()

trainer = pl.Trainer()
test_result = trainer.test(model, dataloaders=test_dataloader, verbose=True)

acc_function = lambda y_hat, y: (y_hat.argmax(dim=-1) == y).float().mean()
ys = []
y_hats = []
for batch in test_dataloader:
    x, y = batch
    y_hat = model(x)
    ys.append(y)
    y_hats.append(y_hat.argmax(dim=-1))
    print(acc_function(y_hat, y))
y = np.concatenate(ys)
y_hat = np.concatenate(y_hats)

confusion_matrix = sklearn.metrics.confusion_matrix(y, y_hat)
confusion_matrix = confusion_matrix / np.sum(confusion_matrix, axis=1)[:, None]
disp = sklearn.metrics.ConfusionMatrixDisplay(
    confusion_matrix=confusion_matrix,
    display_labels=np.array(labels)[np.unique(y)],
)
disp.plot(text_kw={"fontsize": 6})
ax = plt.gca()
ax.tick_params(axis="both", which="major", labelsize=6)
ax.tick_params(axis="both", which="minor", labelsize=6)
plt.xticks(rotation=90)
plt.tight_layout()
plt.savefig("confusion_matrix.pdf")
plt.close()
