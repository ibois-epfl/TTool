"""
Collection of dataset classes for the different data sets.
"""

import pathlib

import numpy as np
import pandas as pd
import skimage
import torch
import torchvision

# List of all tools based on the first dataset
# labels = [
#     "auger_bit_24_400",
#     "auger_drill_bit_20_450",
#     "chain_saw_blade_f_250",
#     "self_feeding_bit_50",
#     "spade_drill_bit_35",
#     "auger_drill_bit_20_235",
#     "auger_drill_bit_30_400",
#     "circular_saw_blade_makita_190",
#     "self_feeding_drill_bit_30_90",
#     "twist_drill_bit_32_90",
#     "auger_drill_bit_20_400",
#     "brad_point_drill_bit_20_150",
#     "saber_saw_blade",
#     "spade_drill_bit_25",
# ]

# List of tools currently in TTool
labels = [
    "auger_drill_bit_20_235",
    "brad_point_drill_bit_20_150",
    "chain_saw_blade_f_250",
    "circular_saw_blade_makita_190",
    "saber_saw_blade",
    "self_feeding_bit_40",
    "self_feeding_bit_50",
    "spade_drill_bit_25",
    "twist_drill_bit_32_90",
]

# Map used to homogenise the labels between the
# different data sets.
mapping = {
    "auger_drill_bit_20_400": "auger_drill_bit_20_400",
    "auger_drill_bit_20_450": "auger_drill_bit_20_450",
    "auger_drill_bit_20_235": "auger_drill_bit_20_235",
    "auger_bit_24_400": "auger_bit_24_400",
    "auger_drill_bit_30_400": "auger_drill_bit_30_400",
    "brad_point_drill_bit_20_150": "brad_point_drill_bit_20_150",
    "chain_saw_blade_f_250": "chain_saw_blade_f_250",
    "circular_sawblade_140": "circular_saw_blade_makita_190",
    "circular_saw_blade_makita_190": "circular_saw_blade_makita_190",
    "drill_oblique_hole_bit_40": "twist_drill_bit_32_90",
    "drill_auger_bit_25_500": "auger_drill_bit_20_450",
    "drill_auger_bit_20_200": "auger_drill_bit_20_235",
    "drill_hinge_cutter_bit_50": "self_feeding_bit_50",
    "saber_saw_blade": "saber_saw_blade",
    "saber_saw_blade_makita_t": "saber_saw_blade",
    "saber_saw_blade_makita_t_300": "saber_saw_blade",
    "saber_sawblade_t1": "saber_saw_blade",
    "self_feeding_drill_bit_30_90": "self_feeding_drill_bit_30_90",
    "self_feeding_bit_40_90": "self_feeding_bit_40",
    "self_feeding_bit_50": "self_feeding_bit_50",
    "self_feeding_bit_50_90": "self_feeding_bit_50",
    "spade_drill_bit_25": "spade_drill_bit_25",
    "spade_drill_bit_25_150": "spade_drill_bit_25",
    "spade_drill_bit_35": "spade_drill_bit_35",
    "st_screw_45": "NA",
    "st_screw_80": "NA",
    "st_screw_100": "NA",
    "st_screw_120": "NA",
    "twist_drill_bit_32_90": "twist_drill_bit_32_90",
    "twist_drill_bit_32_165": "twist_drill_bit_32_90",
}


class ToolDataset(torch.utils.data.Dataset):
    def __init__(
        self,
        img_dir,
        transform=None,
        target_transform=None,
        subsampling=1,
    ):
        """
        Data set class that loads png images from a folder.
        The images have to respect the following naming
        convention: tool_name__ect.png, where the name of the
        tool comes first and is seperated by a double underscore
        from the rest of the file name.

        Parameters
        ----------
        img_dir : pathlib.Path
            Path to folder with png images.
        transform : function
            Function applied to the image, e.g.
            to normalize and/or augment the images.
            Input and output of the function should be
            and image.
        target_transform : function
            Function used to convert the target values.
            Usually this is a function that converts the tool
            name string into a one hot vector encoding.
        subsampling : ing
            Integer used to subsample the data by only using every
            nth image.
        """
        self.img_dir = img_dir

        # Get list of all image paths
        self.img_paths = sorted(list(img_dir.glob("*.png")))

        # Subsample
        self.img_paths = self.img_paths[::subsampling]

        self.transform = transform
        self.target_transform = target_transform

        # Define the function used to extract the ground truth tool name
        # from the file name
        self.get_tool = lambda x: x.stem.split("__")[0]

        # Remove tools that are not in labels
        selected_img_paths = []
        for img_path in self.img_paths:
            new_name = mapping[self.get_tool(img_path)]
            if new_name in labels:
                selected_img_paths.append(img_path)

        self.img_paths = selected_img_paths

    def __len__(self):
        return len(self.img_paths)

    def __getitem__(self, idx):
        img_path = self.img_paths[idx]
        image = torchvision.io.read_image(str(img_path))
        # Convert image to float
        image = image.float() / 255

        # Extract ground truth tool name
        label = self.get_tool(img_path)

        # Homogenize tool name to fit with the convention used here
        label = mapping[label]

        if self.transform:
            # Apply normalization and augmentation if provided
            image = self.transform(image)
        if self.target_transform:
            # Convert tool name string into target for network (usually one hot encoding)
            label = self.target_transform(label)
        return image, label


class FabricationDataset(torch.utils.data.Dataset):
    def __init__(
        self,
        data_dir,
        transform=None,
        target_transform=None,
        videos=None,
        subsampling=1,
    ):
        """
        Data set class for loading the data acquired during fabrication.
        This means the data set should have a `labels.csv` and png files.

        Parameters
        ----------
        data_dir : pathlib.Path
            Directory containing the csv and png files.
        transform : function
            Function applied to the image, e.g.
            to normalize and/or augment the images.
            Input and output of the function should be
            and image.
        target_transform : function
            Function used to convert the target values.
            Usually this is a function that converts the tool
            name string into a one hot vector encoding.
        videos : list of strings
            Names of videos to be used as defined in `labels.csv`.
            This provides a way of splitting the data into training
            and validation data based on the video names.
            If `None`, use all videos.
        subsampling : ing
            Integer used to subsample the data by only using every
            nth frame of each video.
        """
        self.data_dir = pathlib.Path(data_dir)
        self.annotations = pd.read_csv(data_dir / "labels.csv", index_col=0)

        # Homogenize tool name to fit with the convention used here
        self.annotations["Tool"] = self.annotations["Tool"].map(mapping)

        # Remove tools that are not in labels
        self.annotations = self.annotations.loc[
            self.annotations["Tool"].isin(labels), :
        ]

        if videos is not None:
            # Select subset of video files
            self.annotations = self.annotations.loc[
                self.annotations["Video"].isin(videos), :
            ]

        # Subsample videos
        self.annotations = self.annotations.groupby(
            ["Tool", "Video"], group_keys=False
        ).apply(lambda x: x.iloc[::subsampling, :])

        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return self.annotations.shape[0]

    def __getitem__(self, idx):
        # Extract information from labels dataframe
        row = self.annotations.iloc[idx]
        label = row["Tool"]
        video = row["Video"]
        frame = row["Frame"]

        # Load image as float
        img_path = self.data_dir / f"{video:02}_{frame:09}.png"
        image = torchvision.io.read_image(str(img_path))
        image = image.float() / 255

        if self.transform:
            # Apply normalization and augmentation if provided
            image = self.transform(image)
        if self.target_transform:
            # Convert tool name string into target for network (usually one hot encoding)
            label = self.target_transform(label)
        return image, label


def label_transform(label):
    """
    Converts the name of a tool into a one hot
    vector encoding.
    """
    return labels.index(label)


# Composition of transforms used to augment the data.
# This can be passed to any data set class as the transform
# parameters after it is composed with the correct normalizing
# transform for the network.
augmentation_transforms = torchvision.transforms.Compose(
    [
        torchvision.transforms.RandomRotation(degrees=30),
        torchvision.transforms.RandomHorizontalFlip(),
        torchvision.transforms.RandomVerticalFlip(),
        torchvision.transforms.ColorJitter(hue=0.3),
    ]
)


class ToolheadDemoDataset(torch.utils.data.Dataset):
    def __init__(self, data_dir, transform, target_transform):
        """
        Load data consisting of png files within subfolders
        for each tool.

        Parameters
        ----------
        data_dir : pathlib.Path
            Directory containing a subdirectories per tool,
            each of which contains png files.
        transform : function
            Function applied to the image, e.g.
            to normalize and/or augment the images.
            Input and output of the function should be
            and image.
        target_transform : function
            Function used to convert the target values.
            Usually this is a function that converts the tool
            name string into a one hot vector encoding.
        """
        self.data_dir = pathlib.Path(data_dir)
        self.img_paths = list(self.data_dir.glob("*/*.png"))
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.img_paths)

    def __getitem__(self, idx):
        img_path = self.img_paths[idx]

        # Extract tool name based on the parent folder of the image
        label = img_path.parent.stem
        # Remove additional info contained in folder names
        label = label.rstrip("whitebalanced_light")

        # Homogenize tool name to fit with the convention used here
        label = mapping[label]

        image = torchvision.io.read_image(str(img_path))
        image = image.float() / 255
        if self.transform:
            # Apply normalization and augmentation if provided
            image = self.transform(image)
        if self.target_transform:
            # Convert tool name string into target for network (usually one hot encoding)
            label = self.target_transform(label)
        return image, label


class SyntheticDataset(torch.utils.data.Dataset):
    def __init__(self, data_dir, transform, target_transform, subdir):
        """
        Load data consisting of png files within subfolders
        for each tool. In the `data_dir` the folder structure
        shoud be `{tool_name}/kept_image/{subdir}/*.png`.

        Parameters
        ----------
        data_dir : pathlib.Path
            Directory containing a subdirectories per tool,
            each of which contains png files in kept_image/{subdir}.
        transform : function
            Function applied to the image, e.g.
            to normalize and/or augment the images.
            Input and output of the function should be
            and image.
        target_transform : function
            Function used to convert the target values.
            Usually this is a function that converts the tool
            name string into a one hot vector encoding.
        subdir : str
            Name of the subdirectory to use, normally `train` or `val`.
        """
        self.data_dir = pathlib.Path(data_dir)
        self.img_paths = list(self.data_dir.glob(f"*/kept_images/{subdir}/*.png"))
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.img_paths)

    def __getitem__(self, idx):
        img_path = self.img_paths[idx]
        # Extract tool name based on the parent folder of the image
        label = img_path.parents[2].stem
        # Homogenize tool name to fit with the convention used here
        label = mapping[label]

        image = skimage.io.imread(str(img_path))
        # Convert BGR to RGB
        image = np.rollaxis(image, 2, 0)
        image = torch.tensor(image)
        image = image.float() / 255

        if self.transform:
            # Apply normalization and augmentation if provided
            image = self.transform(image)
        if self.target_transform:
            # Convert tool name string into target for network (usually one hot encoding)
            label = self.target_transform(label)
        return image, label
