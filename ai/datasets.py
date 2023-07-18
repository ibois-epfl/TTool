import pathlib

import pandas as pd
import torch
import torchvision

# List of all tools based on the first dataset
labels = [
    "auger_bit_24_400",
    "auger_drill_bit_20_450",
    "chain_saw_blade_f_250",
    "self_feeding_bit_50",
    "spade_drill_bit_35",
    "auger_drill_bit_20_235",
    "auger_drill_bit_30_400",
    "circular_saw_blade_makita_190",
    "self_feeding_drill_bit_30_90",
    "twist_drill_bit_32_90",
    "auger_drill_bit_20_400",
    "brad_point_drill_bit_20_150",
    "saber_saw_blade",
    "spade_drill_bit_25",
]

# Map used to homogenise the labels between the
# first data set and the labels of the fabrication data set
mapping = {
    "auger_bit_24_400": "auger_bit_24_400",
    "auger_drill_bit_20_450": "auger_drill_bit_20_450",
    "chain_saw_blade_f_250": "chain_saw_blade_f_250",
    "self_feeding_bit_50": "self_feeding_bit_50",
    "spade_drill_bit_35": "spade_drill_bit_35",
    "auger_drill_bit_20_235": "auger_drill_bit_20_235",
    "auger_drill_bit_30_400": "auger_drill_bit_30_400",
    "circular_saw_blade_makita_190": "circular_saw_blade_makita_190",
    "self_feeding_drill_bit_30_90": "self_feeding_drill_bit_30_90",
    "twist_drill_bit_32_90": "twist_drill_bit_32_90",
    "auger_drill_bit_20_400": "auger_drill_bit_20_400",
    "brad_point_drill_bit_20_150": "brad_point_drill_bit_20_150",
    "saber_saw_blade": "saber_saw_blade",
    "spade_drill_bit_25": "spade_drill_bit_25",
    "circular_sawblade_140": "circular_saw_blade_makita_190",
    "saber_sawblade_t1": "saber_saw_blade",
    "drill_oblique_hole_bit_40": "twist_drill_bit_32_90",
    "drill_auger_bit_25_500": "auger_drill_bit_20_450",
    "drill_auger_bit_20_200": "auger_drill_bit_20_235",
    "drill_hinge_cutter_bit_50": "self_feeding_bit_50",
    "st_screw_120": "NA",
    "st_screw_100": "NA",
    "st_screw_80": "NA",
    "st_screw_45": "NA",
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

    def __len__(self):
        return len(self.img_paths)

    def __getitem__(self, idx):
        img_path = self.img_paths[idx]
        image = torchvision.io.read_image(str(img_path))
        image = image.float() / 255
        label = img_path.stem.split("__")[0]
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
