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

# List of tools in TTool
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
# first data set and the labels of the fabrication data set
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
        self.img_dir = img_dir
        self.img_paths = sorted(list(img_dir.glob("*.png")))
        self.img_paths = self.img_paths[::subsampling]
        self.transform = transform
        self.target_transform = target_transform
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
        image = image.float() / 255
        label = self.get_tool(img_path)
        label = mapping[label]
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
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
        self.data_dir = pathlib.Path(data_dir)
        self.annotations = pd.read_csv(data_dir / "labels.csv", index_col=0)
        self.annotations["Tool"] = self.annotations["Tool"].map(mapping)
        self.annotations = self.annotations.loc[
            self.annotations["Tool"].isin(labels), :
        ]
        if videos is not None:
            self.annotations = self.annotations.loc[
                self.annotations["Video"].isin(videos), :
            ]
        self.annotations = self.annotations.groupby(
            ["Tool", "Video"], group_keys=False
        ).apply(lambda x: x.iloc[::subsampling, :])
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return self.annotations.shape[0]

    def __getitem__(self, idx):
        row = self.annotations.iloc[idx]
        label = row["Tool"]
        video = row["Video"]
        frame = row["Frame"]
        img_path = self.data_dir / f"{video:02}_{frame:09}.png"
        image = torchvision.io.read_image(str(img_path))
        image = image.float() / 255
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
            label = self.target_transform(label)
        return image, label


def label_transform(label):
    return labels.index(label)


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
        self.data_dir = pathlib.Path(data_dir)
        self.img_paths = list(self.data_dir.glob("*/*.png"))
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.img_paths)

    def __getitem__(self, idx):
        img_path = self.img_paths[idx]
        label = img_path.parent.stem
        label = label.rstrip("whitebalanced_light")
        label = mapping[label]

        image = torchvision.io.read_image(str(img_path))
        image = image.float() / 255
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
            label = self.target_transform(label)
        return image, label


class SyntheticDataset(torch.utils.data.Dataset):
    def __init__(self, data_dir, transform, target_transform, subdir):
        self.data_dir = pathlib.Path(data_dir)
        self.img_paths = list(self.data_dir.glob(f"*/kept_images/{subdir}/*.png"))
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.img_paths)

    def __getitem__(self, idx):
        img_path = self.img_paths[idx]
        label = img_path.parents[2].stem
        label = mapping[label]

        image = skimage.io.imread(str(img_path))
        image = np.rollaxis(image, 2, 0)
        image = torch.tensor(image)
        image = image.float() / 255
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
            label = self.target_transform(label)
        return image, label
