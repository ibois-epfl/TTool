import pathlib
import pickle

import cv2
import numpy as np
import skimage
import tqdm

if __name__ == "__main__":
    features = []
    classes = []

    data_dir = pathlib.Path("../../data/undistorted_data")

    for tool_data_dir in data_dir.glob("*"):
        tool_class = tool_data_dir.stem
        print(tool_class)
        vid_files = list(tool_data_dir.glob("*.mp4"))
        for vid_path in tqdm.tqdm(vid_files):
            cap = cv2.VideoCapture(str(vid_path))
            while cap.isOpened():
                ret, frame = cap.read()
                if ret == True:
                    frame_features = skimage.feature.hog(
                        frame,
                        orientations=8,
                        pixels_per_cell=(16, 16),
                        cells_per_block=(1, 1),
                        feature_vector=True,
                        channel_axis=2,
                    )
                    features.append(frame_features)
                    classes.append(tool_class)
                else:
                    break
            cap.release()
    with open("../../data/features.pkl", "wb") as f:
        pickle.dump({"features": np.stack(features), "classes": classes}, f)
