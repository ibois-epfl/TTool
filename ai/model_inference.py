import torch
import cv2 as cv

# EfficientNet input size
scripted_module = torch.jit.load("ai/torchscripts/efficientnet.pt")

cap = cv.VideoCapture(0)
while True:
    image = cap.read()[1]
    cv.imshow("image", image)

    image = cv.resize(image, (480, 480))
    image = cv.cvtColor(image, cv.COLOR_BGR2RGB)
    image = torch.from_numpy(image).permute(2, 0, 1).unsqueeze(0).float()
    image = image / 255.0
    image -= torch.tensor([0.485, 0.456, 0.406]).view(1, 3, 1, 1)
    image /= torch.tensor([0.229, 0.224, 0.225]).view(1, 3, 1, 1)
    output = scripted_module(image)
    print(output)
    cv.waitKey(0)
    