from train import LitClassifier
from models import TransferResNet, TransferEfficientNet
import torch

if __name__ == "__main__":
    network = TransferEfficientNet(num_classes=9)
    module = LitClassifier(network)
    module.load_from_checkpoint("ai/checkpoints/epoch=19-step=3840.ckpt", map_location=torch.device('cpu'), network=network)
    
    script = module.to_torchscript()
    
    torch.jit.save(script, "ai/torchscripts/efficientnet.pt")

