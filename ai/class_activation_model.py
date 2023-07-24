import collections
import pathlib

import numpy as np
import pytorch_grad_cam
import skimage.io
import torch
import torchvision

import datasets
import models

output_dir = pathlib.Path("/data/ENAC/iBOIS/class_activation_map_output")

image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
ckpt = torch.load("./lightning_logs/version_148/checkpoints/epoch=19-step=3840.ckpt")
lightning_state_dict = ckpt["state_dict"]
state_dict = collections.OrderedDict()
for key, value in lightning_state_dict.items():
    key = key.lstrip("net.")
    state_dict[key] = value

model = models.TransferEfficientNet(num_classes=len(datasets.labels))
model.load_state_dict(state_dict)
model.eval()
target_layers = [model.feature_extractor[0][7][0]]
tool_targes = [datasets.label_transform("spade_drill_bit_25")]

img_dir = pathlib.Path("/data/ENAC/iBOIS/test_dataset/images/val")
dataset = datasets.ToolDataset(
    img_dir,
    transform=image_transform,
    target_transform=datasets.label_transform,
)
dataloader = torch.utils.data.DataLoader(dataset, batch_size=1)

indices = {}
for label in datasets.labels:
    indices[label] = 0

with pytorch_grad_cam.GradCAM(
    model=model, target_layers=target_layers, use_cuda=True
) as cam:
    for input_tensor, target in dataloader:
        input_tensor = input_tensor.to("cuda:0")
        grayscale_cam = cam(input_tensor=input_tensor, targets=None)[0, :]
        rgb_img = input_tensor[0].cpu().numpy()
        rgb_img = rgb_img - np.min(rgb_img)
        rgb_img = rgb_img / np.max(rgb_img)
        rgb_img = np.rollaxis(rgb_img, 0, 3)
        rgb_cam = pytorch_grad_cam.utils.image.show_cam_on_image(
            rgb_img, grayscale_cam, use_rgb=True
        )
        tool = datasets.labels[target.cpu().numpy()[0]]
        skimage.io.imsave(output_dir / f"{tool}_{indices[tool]:09}.png", rgb_cam)
        indices[tool] += 1
