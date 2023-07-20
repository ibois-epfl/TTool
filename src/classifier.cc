#include "classifier.hh"

ttool::ML::Classifier::Classifier(std::string modelPath)
{
    m_Module = torch::jit::load(modelPath);
}

int ttool::ML::Classifier::Classify(cv::Mat image)
{
    torch::Tensor tensor;
    Transform(image, tensor);

    torch::Tensor output = m_Module.forward({tensor}).toTensor();
    int pred = output.argmax(1).item<int>();

    return pred;
}

void ttool::ML::Classifier::Transform(cv::Mat& image, torch::Tensor& tensor)
{
    cv::resize(image, image, cv::Size(IMAGE_SIZE, IMAGE_SIZE), 0, 0, cv::INTER_CUBIC); // It should be BILINEAR but it is not supported by OpenCV
    image.convertTo(image, CV_32FC3, 1.0f / 255.0f);
    tensor = torch::from_blob(image.data, {1, IMAGE_SIZE, IMAGE_SIZE, IMAGE_CHANNEL});
    tensor = tensor.permute({0, 3, 1, 2}); // convert to CxHxW

    // According to https://pytorch.org/vision/main/models/generated/torchvision.models.efficientnet_v2_m.html
    tensor[0][0] = tensor[0][0].sub_(0.485).div_(0.229);
    tensor[0][1] = tensor[0][1].sub_(0.456).div_(0.224);
    tensor[0][2] = tensor[0][2].sub_(0.406).div_(0.225);
}