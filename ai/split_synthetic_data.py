import pathlib

import numpy as np

data_dir = pathlib.Path("/data/ENAC/iBOIS/ToolheadSyntheticDataset")

for tool_dir in data_dir.glob("*"):
    if not tool_dir.is_dir():
        continue
    kept_dir = tool_dir / "kept_images"

    images = list(kept_dir.glob("*.png"))
    n_val = int(len(images) * 0.05)
    val_images = np.random.choice(images, size=n_val, replace=False)

    val_dir = kept_dir / "val"
    train_dir = kept_dir / "train"
    val_dir.mkdir(exist_ok="True")
    train_dir.mkdir(exist_ok="True")

    for image in val_images:
        destination = val_dir / image.name
        image.rename(destination)

    for image in kept_dir.glob("*.png"):
        destination = train_dir / image.name
        image.rename(destination)
