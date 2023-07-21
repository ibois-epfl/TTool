import pathlib
import subprocess

import pandas as pd

from train import mapping

out_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_videos_for_inspection")
data_dir = pathlib.Path("/data/ENAC/iBOIS/labeled_fabrication_images")
annotations = pd.read_csv(data_dir / "labels.csv", index_col=0)
annotations["Tool"] = annotations["Tool"].map(mapping)

for index, df in annotations.groupby(["Video", "Tool", "UID"]):
    vid_id = index[0]
    tool = index[1]
    uid = index[2]
    if tool == "NA":
        continue
    input_pattern = str(data_dir / f"{vid_id:02}_%9d.png")
    output_file = str(out_dir / f"{tool}_{vid_id}_{uid}.mp4")
    start_number = df["Frame"].min() - 90
    if start_number < 0:
        continue
    n_frames = 90
    # n_frames = df["Frame"].max() - start_number
    subprocess.run(
        [
            "ffmpeg",
            "-start_number",
            str(start_number),
            "-i",
            input_pattern,
            "-c:v",
            "libx264",
            "-pix_fmt",
            "yuv420p",
            "-frames:v",
            str(n_frames),
            output_file,
        ]
    )
