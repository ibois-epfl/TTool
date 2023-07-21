import torch

# EfficientNet input size
inp = torch.rand(5, 3, 480, 480)
scripted_module = torch.jit.load("ai/torchscripts/efficientnet.pt")
output = scripted_module(inp)
print(output.shape)