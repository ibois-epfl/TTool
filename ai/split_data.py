import pathlib
import shutil

import numpy as np

img_dir = pathlib.Path("/home/zholmaga/EPFL/TTool/ai/data/ENAC/iBOIS/test_dataset/images/")
print("Splitting data")
image_paths = list(img_dir.glob("*.png"))
print(f"Found {len(image_paths)} images")
tools = [x.stem.split("__")[0] for x in image_paths]
for tool in np.unique(tools):
    print(tool)
    for vid_idx in range(1, 10):
        images = list(img_dir.glob(f"{tool}__0{vid_idx}_*.png"))
        print(f"{tool}__0{vid_idx}_*.png")
        if len(images) == 0:
            break
        images = sorted(images)

        # Chop of first and last 4 seconds
        #images = images[120:-120]

        n = len(images)
        print(n)
        n_test = int(n * 0.1)
        n_val = int(n * 0.1)
        n_train = n - n_test - n_val

        # Take half of the training data from the beginning and the other half from the end
        # Pad by one second to prevent leakage
        n_train_half = n_train // 2
        train_images = images[: n_train_half - 30] + images[-(n_train_half - 30) :]
        print(len(train_images))

        # Test and validation data come from the middle of the video
        test_val_images = images[n_train_half:-n_train_half]
        print(len(test_val_images))
        test_images = test_val_images[:n_test]
        print(len(test_images))
        val_images = test_val_images[n_test:]
        print(len(val_images))


        try:
            for image in train_images:
                shutil.move(image, "/home/zholmaga/EPFL/TTool/ai/data/ENAC/iBOIS/images/train")
            for image in test_images:
                shutil.move(image, "/home/zholmaga/EPFL/TTool/ai/data/ENAC/iBOIS/images/test")
            for image in val_images:
                shutil.move(image, "/home/zholmaga/EPFL/TTool/ai/data/ENAC/iBOIS/images/val")
        except:
            pass


