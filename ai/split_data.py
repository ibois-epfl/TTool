import pathlib
import sklearn.model_selection
import numpy as np
import shutil

img_dir = pathlib.Path("/data/ENAC/iBOIS/images")
image_paths = list(img_dir.glob("*.png"))
X = np.zeros((len(image_paths), 1))
tools = [x.stem.split("__")[0] for x in image_paths]
train_idx, test_idx = sklearn.model_selection.train_test_split(np.arange(len(tools)), test_size=0.05, stratify=tools, random_state=42)

for idx in test_idx:
    shutil.move(image_paths[idx], "/data/ENAC/iBOIS/images/val")
