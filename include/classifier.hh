#pragma once

#include <torch/script.h> 
#include <opencv2/opencv.hpp>

namespace ttool::ML
{
    #define IMAGE_SIZE 480
    #define IMAGE_CHANNEL 3
    #define NUM_CLASSES 14

    class Classifier
    {
        public:
        /**
         * @brief Construct a new Classifier object
         * 
         * @param modelPath path to the model file
         */
        Classifier(std::string modelPath);

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
        std::string m_Pred2Label[NUM_CLASSES] = {
            "auger_bit_24_400",
            "auger_drill_bit_20_450",
            "chain_saw_blade_f_250",
            "self_feeding_bit_50",
            "spade_drill_bit_35",
            "auger_drill_bit_20_235",
            "auger_drill_bit_30_400",
            "circular_saw_blade_makita_190",
            "self_feeding_drill_bit_30_90",
            "twist_drill_bit_32_90",
            "auger_drill_bit_20_400",
            "brad_point_drill_bit_20_150",
            "saber_saw_blade",
            "spade_drill_bit_25"
        };
    };
}