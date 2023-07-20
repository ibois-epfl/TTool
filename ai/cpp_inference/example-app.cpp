#include <torch/script.h> // One-stop header.
#include <opencv2/opencv.hpp>

#include <iostream>
#include <memory>

#define IMAGE_SIZE 480
#define IMAGE_CHANNEL 3
#define NUM_CLASSES 14


bool LoadImage(std::string imagePath, cv::Mat& image)
{
  image = cv::imread(imagePath, cv::IMREAD_COLOR);
  if (image.empty() || !image.data)
  {
    std::cout << "Could not read the image: " << imagePath << std::endl;
    return false;
  }
  cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

  return true;
}

/**
 * @brief Transform the image to tensor and normalize it according to EfficientNet_v2 model
 * 
 * @param image   The input image (RGB format)
 * @param tensor  The output tensor
 */
void Transform(cv::Mat& image, torch::Tensor& tensor)
{
  cv::resize(image, image, cv::Size(IMAGE_SIZE, IMAGE_SIZE), 0, 0, cv::INTER_CUBIC); // It should be BILINEAR but it is not supported by OpenCV
  image.convertTo(image, CV_32FC3, 1.0f / 255.0f);
  tensor = torch::from_blob(image.data, {1, IMAGE_SIZE, IMAGE_SIZE, IMAGE_CHANNEL});
  tensor = tensor.permute({0, 3, 1, 2}); // convert to CxHxW

  // mean=[0.485, 0.456, 0.406] and std=[0.229, 0.224, 0.225]
  tensor[0][0] = tensor[0][0].sub_(0.485).div_(0.229);
  tensor[0][1] = tensor[0][1].sub_(0.456).div_(0.224);
  tensor[0][2] = tensor[0][2].sub_(0.406).div_(0.225);
}

int main(int argc, const char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "usage: example-app <path-to-exported-script-module>\n";
    return -1;
  }


  torch::jit::script::Module module;
  try
  {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load(argv[1]);

    cv::Mat image;
    if (LoadImage("test.png", image) == false)
    {
      return -1;
    }
    torch::Tensor inp;
    Transform(image, inp);

    torch::Tensor output = module.forward({inp}).toTensor();
    std::cout << output.size(0) << " " << output.size(1) << std::endl;
    std::cout << output << std::endl;
  }
  catch (const c10::Error& e)
  {
    std::cerr << "error loading the model\n";
    return -1;
  }

  std::cout << "ok\n";
}