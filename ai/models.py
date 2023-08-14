"""
Collection of models
"""

import torch.nn as nn
import torchvision


class ResNetBlock(nn.Module):
    def __init__(self, c_in, act_fn, subsample=False, c_out=-1):
        """
        Block used for constructing the ResNet.
        """
        super().__init__()
        if not subsample:
            c_out = c_in
            stride = 1
        else:
            stride = 2

        self.net = nn.Sequential(
            nn.Conv2d(c_in, c_out, kernel_size=3, padding=1, stride=stride, bias=False),
            nn.BatchNorm2d(c_out),
            act_fn(),
            nn.Conv2d(c_out, c_out, kernel_size=3, padding=1, bias=False),
            nn.BatchNorm2d(c_out),
        )

        if subsample:
            self.downsample = nn.Conv2d(c_in, c_out, kernel_size=1, stride=2)
        else:
            self.downsample = None
        self.act_fn = act_fn()

    def forward(self, x):
        z = self.net(x)
        if self.downsample is not None:
            x = self.downsample(x)
        out = z + x
        out = self.act_fn(out)
        return out


class ResNet(nn.Module):
    def __init__(
        """
        Implementaiton of the ResNet model.
        The parameters can be used to change the architecture.
        """
        self,
        num_classes=14,
        num_blocks=[3, 3, 3],
        c_hidden=[16, 32, 64],
        act_fn=nn.ReLU,
        **kwargs,
    ):
        super().__init__()
        self.input_net = nn.Sequential(
            nn.Conv2d(3, c_hidden[0], kernel_size=3, padding=1, bias=False),
            nn.BatchNorm2d(c_hidden[0]),
            act_fn(),
        )
        blocks = []
        for block_idx, block_count in enumerate(num_blocks):
            for bc in range(block_count):
                if bc == 0 and block_idx > 0:
                    subsample = True
                    c_in = c_hidden[block_idx - 1]
                else:
                    subsample = False
                    c_in = c_hidden[block_idx]
                block = ResNetBlock(
                    c_in=c_in,
                    act_fn=act_fn,
                    subsample=subsample,
                    c_out=c_hidden[block_idx],
                )
                blocks.append(block)
        self.blocks = nn.Sequential(*blocks)

        self.classification_head = nn.Sequential(
            nn.AdaptiveAvgPool2d((1, 1)),
            nn.Flatten(),
            nn.Linear(c_hidden[-1], num_classes),
        )

        # initialise weights
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode="fan_out", nonlinearity="relu")
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def forward(self, x):
        x = self.input_net(x)
        x = self.blocks(x)
        x = self.classification_head(x)
        return x


class TransferResNet(nn.Module):
    def __init__(
        self,
        num_classes=14,
        **kwargs,
    ):
        """
        ResNet18 model with default weights from torchvision
        and a custom classification head to match the number
        of classes we have.
        """
        super().__init__()
        backbone = torchvision.models.resnet18(weights="DEFAULT")
        num_filters = backbone.fc.in_features
        layers = list(backbone.children())[:-2]
        self.feature_extractor = nn.Sequential(*layers).eval()

        self.classification_head = nn.Sequential(
            nn.AdaptiveAvgPool2d((1, 1)),
            nn.Flatten(),
            nn.Linear(num_filters, num_classes),
        )

    def forward(self, x):
        x = self.feature_extractor(x)
        x = self.classification_head(x)
        return x


class TransferEfficientNet(nn.Module):
    def __init__(
        self,
        num_classes=14,
        **kwargs,
    ):
        """
        EfficientNet model with default weights from torchvision
        and a custom classification head to match the number
        of classes we have.
        """
        super().__init__()
        backbone = torchvision.models.efficientnet_v2_s(weights="DEFAULT")
        layers = list(backbone.children())
        num_filters = layers[-1][1].in_features
        self.feature_extractor = nn.Sequential(*layers[:-2]).eval()

        self.classification_head = nn.Sequential(
            nn.AdaptiveAvgPool2d(output_size=1),
            nn.Dropout(p=0.2, inplace=True),
            nn.Flatten(),
            nn.Linear(num_filters, num_classes),
        )

    def forward(self, x):
        x = self.feature_extractor(x)
        x = self.classification_head(x)
        return x
