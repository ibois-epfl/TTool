import pathlib
import shutil

import numpy as np

img_dir = pathlib.Path("/data/ENAC/iBOIS/images")
image_paths = list(img_dir.glob("*.png"))
tools = [x.stem.split("__")[0] for x in image_paths]
for tool in np.unique(tools):
    print(tool)
    for vid_idx in range(1, 10):
        print(f"\t{vid_idx}")
        images = list(img_dir.glob(f"{tool}__{vid_idx}_*.png"))
        if len(images) == 0:
            break
        images = sorted(images)

        # Chop of first and last 4 seconds
        images = images[120:-120]

        n = len(images)
        n_test = int(n * 0.1)
        n_val = int(n * 0.1)
        n_train = n - n_test - n_val

        # Take half of the training data from the beginning and the other half from the end
        # Pad by one second to prevent leakage
        n_train_half = n_train // 2
        train_images = images[: n_train_half - 30] + images[-(n_train_half - 30) :]

        # Test and validation data come from the middle of the video
        test_val_images = images[n_train_half:-n_train_half]
        test_images = test_val_images[:n_test]
        val_images = test_val_images[n_test:]

        for image in train_images:
            shutil.move(image, "/data/ENAC/iBOIS/images/train")
        for image in test_images:
            shutil.move(image, "/data/ENAC/iBOIS/images/test")
        for image in val_images:
            shutil.move(image, "/data/ENAC/iBOIS/images/val")
