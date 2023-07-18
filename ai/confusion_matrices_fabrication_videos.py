import pathlib

import lightning.pytorch as pl
import matplotlib.pyplot as plt
import numpy as np
import sklearn.metrics
import torch
import torchvision
from torch.utils.data import DataLoader

from train import (
    FabricationDataset,
    LitClassifier,
    TransferEfficientNet,
    label_transform,
    labels,
)

torch.set_float32_matmul_precision("high")

image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
network = TransferEfficientNet()

cross_val_dict = {
    # 1: "lightning_logs/version_92/checkpoints/epoch=0-step=146.ckpt",
    # 2: "lightning_logs/version_91/checkpoints/epoch=3-step=488.ckpt",
    # 3: "lightning_logs/version_90/checkpoints/epoch=5-step=540.ckpt",
    # 3: "lightning_logs/version_103/checkpoints/epoch=7-step=720.ckpt",
    # 4: "lightning_logs/version_89/checkpoints/epoch=3-step=492.ckpt",
    # 18: "lightning_logs/version_88/checkpoints/epoch=4-step=660.ckpt",
    # 19: "lightning_logs/version_87/checkpoints/epoch=1-step=266.ckpt",
    # 20: "lightning_logs/version_86/checkpoints/epoch=1-step=268.ckpt",
    # 1: "lightning_logs/version_104/checkpoints/epoch=2-step=99.ckpt",
    # 2: "lightning_logs/version_105/checkpoints/epoch=7-step=208.ckpt",
    # 3: "lightning_logs/version_106/checkpoints/epoch=4-step=130.ckpt",
    # 4: "lightning_logs/version_107/checkpoints/epoch=8-step=225.ckpt",
    # 18: "lightning_logs/version_108/checkpoints/epoch=24-step=750.ckpt",
    # 19: "lightning_logs/version_109/checkpoints/epoch=4-step=150.ckpt",
    # 20: "lightning_logs/version_110/checkpoints/epoch=3-step=120.ckpt",
    20: "lightning_logs/version_122/checkpoints/epoch=19-step=1220.ckpt",
}

for vid_id, ckpt in cross_val_dict.items():
    data_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_images/")
    val_dataset = FabricationDataset(
        data_dir,
        transform=image_transform,
        target_transform=label_transform,
        videos=[vid_id],
    )
    val_dataloader = DataLoader(val_dataset, batch_size=25, num_workers=8)

    model = LitClassifier.load_from_checkpoint(
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
        y, y_hat, labels=np.argsort(labels), normalize="true"
    )
    disp = sklearn.metrics.ConfusionMatrixDisplay(
        confusion_matrix=confusion_matrix,
        display_labels=np.sort(np.array(labels)),  # [np.unique(y)]),
    )
    disp.plot(text_kw={"fontsize": 6})
    ax = plt.gca()
    ax.tick_params(axis="both", which="major", labelsize=6)
    ax.tick_params(axis="both", which="minor", labelsize=6)
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.title(f"Video {vid_id} acc={acc:.2f}")
    plt.savefig(
        # f"confusion_matrices_fabrication_videos/confusion_matrix_{vid_id}.pdf"
        # f"confusion_matrices_fabrication_videos/confusion_matrix_{vid_id}_shifted_labels.pdf"
        f"confusion_matrices_fabrication_videos/confusion_matrix_{vid_id}_shifted_labels_plus_tooldataset.pdf"
    )
    plt.close()
