import argparse

from train import LitClassifier
from models import TransferResNet, TransferEfficientNet
import torch

if __name__ == "__main__":
    # parse arguments
    parser = argparse.ArgumentParser(
        prog="model_export.py",
        description="Export model to torchscript",
    )
    parser.add_argument('-n', '--num_classes', type=int, help='number of classes')
    parser.add_argument('-c', '--checkpoint', type=str, help='path to checkpoint')
    parser.add_argument('-o', '--output', type=str, help='path to output')

    args = parser.parse_args()

    # load 
    network = TransferEfficientNet(num_classes=args.num_classes)
    module = LitClassifier(network)
    module.load_from_checkpoint(args.checkpoint, map_location=torch.device('cpu'), network=network)
    
    script = module.to_torchscript()
    
    torch.jit.save(script, args.output)

