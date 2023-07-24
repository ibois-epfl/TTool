#pragma once

#include <torch/script.h>
#include <opencv2/opencv.hpp>

namespace ttool::ML
{
    class Classifier
    {
        public:
        /**
         * @brief Construct a new Classifier object
         * 
         * @param modelPath path to the model file
         */
        Classifier(std::string modelPath,
                   int imageSize,
                   int imageChannel,
                   std::vector<std::string> pred2Label,
                   std::vector<float> mean,
                   std::vector<float> std);

        /**
         * @brief Classify the image
         * 
         * @param image 
         * @return int 
         */
        int Classify(cv::Mat image);

        /**
         * @brief Get the label of the prediction
         * 
         * @param prediction 
         * @return std::string 
         */
        std::string GetLabel(int prediction) { return m_Pred2Label[prediction]; }

        private:
        /**
         * @brief Transform the image to tensor and normalize it according to EfficientNet_v2 model
         * 
         * @param image   The input image (RGB format)
         * @param tensor  The output tensor
         */
        void Transform(cv::Mat image, torch::Tensor& tensor);

        private:
        torch::jit::script::Module m_Module;
        std::vector<std::string> m_Pred2Label;
        int IMAGE_SIZE;
        int IMAGE_CHANNEL;
        std::vector<float> m_Mean;
        std::vector<float> m_Std;
    };
}