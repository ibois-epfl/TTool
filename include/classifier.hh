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

#pragma once

#include <torch/script.h>
#include <opencv2/opencv.hpp>

namespace ttool::ML

{
    /**
     * @brief This class is the ML classifier for tool head.
     * 
     */
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
         * Image should be in BGR format
         * 
         * @param image 
         * @return int 
         */
        int Classify(cv::Mat image);

        /**
         * @brief Classify the image and return the top k predictions
         * Image should be in BGR format
         * 
         * @param image 
         * @param k 
         * @return std::vector<std::string> The top k predictions in label names
         */
        std::vector<std::string> ClassifyTopK(cv::Mat image, int k);

        /**
         * @brief Get the label of the prediction
         * 
         * @param prediction 
         * @return std::string 
         */
        std::string GetLabel(int prediction) { return m_Pred2Label[prediction]; }

        /**
         * @brief Get the total number of classes
         * 
         * @return int 
         */
        int GetTotalClass() { return m_Pred2Label.size(); }

        private:
        /**
         * @brief Transform the image to tensor and normalize it according to EfficientNet_v2 model
         * 
         * @param image   The input image (RGB format)
         * @param tensor  The output tensor
         */
        void Transform(cv::Mat image, torch::Tensor& tensor);

        /**
         * @brief Classify the image and return the raw score
         * 
         * @param image 
         * @return std::vector<float> 
         */
        torch::Tensor ClassifyRawScore(cv::Mat image);

        public:
        std::stringstream ClassifierLog;

        private:
        torch::jit::script::Module m_Module;
        std::vector<std::string> m_Pred2Label;
        int IMAGE_SIZE;
        int IMAGE_CHANNEL;
        std::vector<float> m_Mean;
        std::vector<float> m_Std;

        bool m_IsInitialized = false;
    };
}