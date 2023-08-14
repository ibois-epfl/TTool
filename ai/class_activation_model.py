"""
Computes class activation maps for the EfficientNet
architecture on a given data set.
This can be used to evaluate whether the model focuses
on the tools or other features in the image to make its
decision.
"""

import collections
import pathlib

import numpy as np
import pytorch_grad_cam
import skimage.io
import torch
import torchvision

import datasets
import models

# Directory where class activation map images will be stored
output_dir = pathlib.Path("/data/ENAC/iBOIS/class_activation_map_output")

# Load normalization transform and checkpoint
image_transform = torchvision.models.EfficientNet_V2_S_Weights.DEFAULT.transforms()
ckpt = torch.load(
    "/data/ENAC/iBOIS/lightning_logs/version_148/checkpoints/epoch=19-step=3840.ckpt"
)
lightning_state_dict = ckpt["state_dict"]

# net. is a pytorch lighning convention of naming the variable in the state dict
# that is not compatible with pytorch. The 'net.' part has to be removed form the
# keys to be able to use pytorch's `load_state_dict`
state_dict = collections.OrderedDict()
for key, value in lightning_state_dict.items():
    key = key.lstrip("net.")
    state_dict[key] = value

# prepare model for evaluation
model = models.TransferEfficientNet(num_classes=len(datasets.labels))
model.load_state_dict(state_dict)
model.eval()

# Choose the last convolutional layer for the computaiton of the class
# activation
target_layers = [model.feature_extractor[0][7][0]]

# Return the index of the one hot encoding for a given class.
# This will be the class for which the class activation map is computed.
tool_targes = [datasets.label_transform("spade_drill_bit_25")]

# Prepare data set for which the class activation maps will be computes.
img_dir = pathlib.Path("/data/ENAC/iBOIS/test_dataset/images/val")
dataset = datasets.ToolDataset(
    img_dir,
    transform=image_transform,
    target_transform=datasets.label_transform,
)
dataloader = torch.utils.data.DataLoader(dataset, batch_size=1)

# Index appended to the end of the file name. It keeps track of
# of many class activation maps have already been saved for each
# class (label).
indices = {}
for label in datasets.labels:
    indices[label] = 0

# GradCAM is the class that calculates the class activation map
with pytorch_grad_cam.GradCAM(
    model=model, target_layers=target_layers, use_cuda=True
) as cam:
    for input_tensor, target in dataloader:
        # Compute class activation map for the input_tensor
        input_tensor = input_tensor.to("cuda:0")
        grayscale_cam = cam(input_tensor=input_tensor, targets=None)[0, :]

        # Turn input tensor into RGB image
        rgb_img = input_tensor[0].cpu().numpy()
        rgb_img = rgb_img - np.min(rgb_img)
        rgb_img = rgb_img / np.max(rgb_img)
        rgb_img = np.rollaxis(rgb_img, 0, 3)

        # Overlay class activation map on RGB input image
        rgb_cam = pytorch_grad_cam.utils.image.show_cam_on_image(
            rgb_img, grayscale_cam, use_rgb=True
        )

        # Convert index to tool name
        tool = datasets.labels[target.cpu().numpy()[0]]

        # Save output
        skimage.io.imsave(output_dir / f"{tool}_{indices[tool]:09}.png", rgb_cam)

        # Count number of class maps computed for this tool
        indices[tool] += 1
