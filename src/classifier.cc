#include "classifier.hh"

ttool::ML::Classifier::Classifier(std::string modelPath,
                                  int imageSize,
                                  int imageChannel,
                                  std::vector<std::string> pred2Label,
                                  std::vector<float> mean,
                                  std::vector<float> std)
: IMAGE_SIZE(imageSize), IMAGE_CHANNEL(imageChannel), m_Pred2Label(pred2Label), m_Mean(mean), m_Std(std)
{
    m_Module = torch::jit::load(modelPath);

    // Dry run to initialize the model
    cv::Mat image = cv::Mat::zeros(IMAGE_SIZE, IMAGE_SIZE, CV_8UC3);
    torch::Tensor tensor;
    Transform(image, tensor);
    m_Module.forward({tensor}).toTensor();
}

int ttool::ML::Classifier::Classify(cv::Mat image)
{
    torch::Tensor tensor;
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    Transform(image, tensor);

    torch::Tensor output = m_Module.forward({tensor}).toTensor();
    output = output.softmax(1);
    auto topk = torch::topk(output, 3, 1); // (values, indices)
    // for (int i = 0; i < 3; ++i)
    // {
    //     std::cout << "Top " << i << ": " << std::get<0>(topk)[0][i].item<float>() << " and " << GetLabel(std::get<1>(topk)[0][i].item<int>()) << std::endl;
    // }
    int pred = output.argmax(1).item<int>();

    return pred;
}

void ttool::ML::Classifier::Transform(cv::Mat image, torch::Tensor& tensor)
{
    cv::resize(image, image, cv::Size(IMAGE_SIZE, IMAGE_SIZE), 0, 0, cv::INTER_CUBIC); // It should be BILINEAR but it is not supported by OpenCV
    image.convertTo(image, CV_32FC3, 1.0f / 255.0f);
    tensor = torch::from_blob(image.data, {1, IMAGE_SIZE, IMAGE_SIZE, IMAGE_CHANNEL});
    tensor = tensor.permute({0, 3, 1, 2}); // convert to CxHxW

    tensor[0][0] = tensor[0][0].sub_(m_Mean[0]).div_(m_Std[0]);
    tensor[0][1] = tensor[0][1].sub_(m_Mean[1]).div_(m_Std[1]);
    tensor[0][2] = tensor[0][2].sub_(m_Mean[2]).div_(m_Std[2]);
}