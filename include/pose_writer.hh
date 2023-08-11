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

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/core.hpp>


namespace ttool
{
    /**
     * @brief This class is used to write the pose to a file.
     * It can write a pose with or without an image.
     * The pose information comes with the object ID and the object name.   
     * 
     */
    class PoseWriter
    {
        public:
            PoseWriter(){};

            /**
             * @brief Construct a new PoseWriter object
             * 
             * @param filename   path to the output file
             * @param configFile path to the config file
             * @param modelFiles list of model files
             */
            PoseWriter(const std::string& filename, const std::string &configFile, const std::vector<std::string> &modelFiles)
            : m_ModelFiles(modelFiles)
            {
                m_OutputFile.open(filename);
                if (!m_OutputFile.is_open())
                {
                    std::cerr << "Cannot open the pose file";
                    exit(1);
                }
                // print time stamp and commit hash
                m_OutputFile << "timestamp: " << __DATE__ << " " << __TIME__ << std::endl;
                m_OutputFile << "config_file: " << configFile << std::endl;
                m_OutputFile << "model_files: " << std::endl;
                int objectID = 1;
                for (const auto& modelFile : m_ModelFiles)
                {
                    m_OutputFile << "\tmodelID (" << objectID++ << ")" << modelFile << std::endl;
                }

            }
            ~PoseWriter()
            {
                m_OutputFile.close();
            }

            /**
             * @brief Write the pose to the file
             * 
             * @param pose 
             * @param objectID 
             */
            void Write(cv::Matx44f& pose, int objectID)
            {
                m_OutputFile << "objectID: " << objectID << std::endl;
                m_OutputFile << "pose: " << std::endl;
                m_OutputFile << '\t' << pose(0, 0) << ' ' << pose(0, 1) << ' ' << pose(0, 2) << ' ' << pose(0, 3) << std::endl;
                m_OutputFile << '\t' << pose(1, 0) << ' ' << pose(1, 1) << ' ' << pose(1, 2) << ' ' << pose(1, 3) << std::endl;
                m_OutputFile << '\t' << pose(2, 0) << ' ' << pose(2, 1) << ' ' << pose(2, 2) << ' ' << pose(2, 3) << std::endl;
                m_OutputFile << '\t' << pose(3, 0) << ' ' << pose(3, 1) << ' ' << pose(3, 2) << ' ' << pose(3, 3) << std::endl;
            }

            void SetImageDir(const std::string& imageDir)
            {
                m_ImageDir = imageDir;
            }

            /**
             * @brief Write the pose to the file with an image
             * 
             * @param pose 
             * @param objectID 
             * @param image
             */
            void Write(cv::Matx44f& pose, int objectID, std::string objectName, cv::Mat image)
            {
                char buffer[9];
                m_OutputFile << "\n";
                std::snprintf(buffer, sizeof(buffer), "%08d", m_FrameID++);
                std::string filename = m_ImageDir + "/" + buffer + ".png";
                cv::imwrite(filename, image);

                Write(pose, objectID);
                m_OutputFile << "object_name: " << objectName << std::endl;
                m_OutputFile << "image: " << filename << std::endl;
            }

        private:
            int m_FrameID = 0;
            std::string m_ImageDir;
            std::ofstream m_OutputFile;
            std::vector<std::string> m_ModelFiles;
    };
}