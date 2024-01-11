/**
 * TTool
 * Copyright (C) 2023  Andrea Settimi, Naravich Chutisilp (IBOIS, EPFL)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "classifier.hh"

ttool::ML::Classifier::Classifier(std::string modelPath,
                                  int imageSize,
                                  int imageChannel,
                                  std::vector<std::string> pred2Label,
                                  std::vector<float> mean,
                                  std::vector<float> std)
: IMAGE_SIZE(imageSize), IMAGE_CHANNEL(imageChannel), m_Pred2Label(pred2Label), m_Mean(mean), m_Std(std)
{
    torch::Device device(torch::kCPU);
    m_Module = torch::jit::load(modelPath, device);

    // // Dry run to initialize the model
    // cv::Mat image = cv::Mat::zeros(IMAGE_SIZE, IMAGE_SIZE, CV_8UC3);
    // torch::Tensor tensor;
    // Transform(image, tensor);
    // m_Module.forward({tensor}).toTensor();
}

int ttool::ML::Classifier::Classify(cv::Mat image)
{
    torch::Tensor output = ClassifyRawScore(image);

    output = output.softmax(1);
    auto topk = torch::topk(output, 3, 1); // (values, indices)
    ClassifierLog.str("");
    for (int i = 0; i < 3; ++i)
    {
        ClassifierLog << "Top " << i << ": " << std::get<0>(topk)[0][i].item<float>() << " and " << GetLabel(std::get<1>(topk)[0][i].item<int>()) << std::endl;
    }
    int pred = output.argmax(1).item<int>();

    return pred;
}

torch::Tensor ttool::ML::Classifier::ClassifyRawScore(cv::Mat image)
{
    if (!m_IsInitialized)
    {
        cv::Mat imageFirstRun;
        image.copyTo(imageFirstRun);
        cv::cvtColor(imageFirstRun, imageFirstRun, cv::COLOR_BGR2RGB);
        torch::Tensor tensor;
        Transform(imageFirstRun, tensor);
        m_Module.forward({tensor}).toTensor();
        m_IsInitialized = true;
    }

    torch::Tensor tensor;
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    Transform(image, tensor);

    torch::Tensor output = m_Module.forward({tensor}).toTensor();

    return output;
}

std::vector<std::string> ttool::ML::Classifier::ClassifyTopK(cv::Mat image, int k)
{
    torch::Tensor output = ClassifyRawScore(image);

    output = output.softmax(1);
    auto topk = torch::topk(output, k, 1); // (values, indices)
    std::vector<std::string> result;
    for (int i = 0; i < k; ++i)
    {
        result.push_back(GetLabel(std::get<1>(topk)[0][i].item<int>()));
    }

    ClassifierLog.str("");
    for (int i = 0; i < k; ++i)
    {
        ClassifierLog << "Top " << i << ": " << std::get<0>(topk)[0][i].item<float>() << " and " << GetLabel(std::get<1>(topk)[0][i].item<int>()) << std::endl;
    }

    return result;
}

void ttool::ML::Classifier::Transform(cv::Mat image, torch::Tensor& tensor)
{
    int side = std::min(image.cols, image.rows);
    int x = (image.cols - side) / 2;
    int y = (image.rows - side) / 2;
    cv::Rect roi(x, y, side, side);
    image = image(roi);

    cv::resize(image, image, cv::Size(IMAGE_SIZE, IMAGE_SIZE), 0, 0, cv::INTER_CUBIC); // It should be BILINEAR but it is not supported by OpenCV
    image.convertTo(image, CV_32FC3, 1.0f / 255.0f);
    tensor = torch::from_blob(image.data, {1, IMAGE_SIZE, IMAGE_SIZE, IMAGE_CHANNEL});
    tensor = tensor.permute({0, 3, 1, 2}); // convert to CxHxW

    tensor[0][0] = tensor[0][0].sub_(m_Mean[0]).div_(m_Std[0]);
    tensor[0][1] = tensor[0][1].sub_(m_Mean[1]).div_(m_Std[1]);
    tensor[0][2] = tensor[0][2].sub_(m_Mean[2]).div_(m_Std[2]);
}