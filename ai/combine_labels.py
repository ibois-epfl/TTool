"""
Combine all of the fabrication labels into one file
and shift them forward by 90 frames and limit them to 60 frames.
"""

import pathlib

import numpy as np
import pandas as pd

data_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_videos")
img_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_images")

label_files = data_dir.glob("*/*label_sequence_manual_corrected.txt")

frames = []
video_indices = []
tools = []
uid = []
uid_val = 0

for label_file in label_files:
    stem = label_file.stem
    video_index = int(stem.split("_")[0])
    df = pd.read_table(
        label_file, delimiter="-", header=13, names=["start", "stop", "tool"]
    )
    for _, row in df.iterrows():
        start, stop, tool = row
        start = start - 90
        if start < 0:
            continue
        stop = start + 60
        length = stop - start + 1
        frames.extend(np.arange(start, stop + 1))
        video_indices.extend([video_index] * length)
        tools.extend([tool] * length)
        uid.extend([uid_val] * length)
        uid_val += 1

df = pd.DataFrame({"Frame": frames, "Video": video_indices, "Tool": tools, "UID": uid})

df.to_csv(img_dir / "labels.csv")
df.to_csv(data_dir / "labels.csv")
