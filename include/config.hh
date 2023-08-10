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

#define SET_UNORDERED_MAP_VALUE(unorderedMap, key, value) \
    auto it = unorderedMap.find(key); \
    if (it != unorderedMap.end()) \
        it->second.get() = value;

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <functional>
#include <unordered_map>

namespace ttool
{
    struct ConfigData
    {
        public:
            // Model
            std::vector<std::string> ModelFiles;
            std::vector<std::string> AcitFiles;
            std::vector<std::vector<float>> GroundTruthPoses;

            // Histogram
            int HistOffset;
            int HistRad;
            int SearchRad;

            // Visualizer
            float Zn;
            float Zf;

            // ML
            std::string ClassifierModelPath;
            std::vector<std::string> ClassifierLabels;
            int ClassifierImageSize;
            int ClassifierImageChannels;
            std::vector<float> ClassifierMean;
            std::vector<float> ClassifierStd;

            // Unordered maps are used to help Setters more dynamic typed
            std::unordered_map<std::string, std::reference_wrapper<std::vector<std::string>>> stringVectorMembers = {
                {"modelFiles", std::ref(ModelFiles)},
                {"acitFiles", std::ref(AcitFiles)},
                {"classifierLabels", std::ref(ClassifierLabels)}
            };

            std::unordered_map<std::string, std::reference_wrapper<std::vector<float>>> floatVectorMembers = {
                {"classifierMean", std::ref(ClassifierMean)},
                {"classifierStd", std::ref(ClassifierStd)}
            };

            std::unordered_map<std::string, std::reference_wrapper<std::vector<std::vector<float>>>> floatVectorVectorMembers = {
                {"groundTruthPoses", std::ref(GroundTruthPoses)}
            };

            std::unordered_map<std::string, std::reference_wrapper<int>> intMembers = {
                {"histOffset", std::ref(HistOffset)},
                {"histRad", std::ref(HistRad)},
                {"searchRad", std::ref(SearchRad)},
                {"classifierImageSize", std::ref(ClassifierImageSize)},
                {"classifierImageChannels", std::ref(ClassifierImageChannels)}
            };

            std::unordered_map<std::string, std::reference_wrapper<float>> floatMembers = {
                {"zn", std::ref(Zn)},
                {"zf", std::ref(Zf)}
            };

            std::unordered_map<std::string, std::reference_wrapper<std::string>> stringMembers = {
                {"classifierModelPath", std::ref(ClassifierModelPath)}
            };

            /**
             * @brief Set the Value object
             * 
             * @param key key of the value to set
             * @param value value to set
             */
            void setValue(std::string key, std::vector<std::string> value)
            {
                SET_UNORDERED_MAP_VALUE(stringVectorMembers, key, value);
                return;
                if (auto it = stringVectorMembers.find(key); it != stringVectorMembers.end())
                {
                    it->second.get().clear();
                    it->second.get() = value;
                }
            }

            void setValue(std::string key, std::vector<float> value)
            {
                SET_UNORDERED_MAP_VALUE(floatVectorMembers, key, value);
                return;
                if (auto it = floatVectorMembers.find(key); it != floatVectorMembers.end())
                {
                    it->second.get().clear();
                    it->second.get() = value;
                }
            }

            /**
             * @brief Set the Value object
             * 
             * @param key key of the value to set
             * @param value value to set
             */
            void setValue(std::string key, std::vector<std::vector<float>> value)
            {
                SET_UNORDERED_MAP_VALUE(floatVectorVectorMembers, key, value);
                return;
                if (auto it = floatVectorVectorMembers.find(key); it != floatVectorVectorMembers.end())
                {
                    it->second.get().clear();
                    it->second.get() = value;
                }
            }

            /**
             * @brief Set the Value object
             * 
             * @param key key of the value to set
             * @param value value to set
             */
            void setValue(std::string key, int value)
            {
                SET_UNORDERED_MAP_VALUE(intMembers, key, value);
                return;
                if (auto it = intMembers.find(key); it != intMembers.end())
                {
                    it->second.get() = value;
                }
            }

            /**
             * @brief Set the Value object
             * 
             * @param key key of the value to set
             * @param value value to set
             */
            void setValue(std::string key, float value)
            {
                SET_UNORDERED_MAP_VALUE(floatMembers, key, value);
                return;
                if (auto it = floatMembers.find(key); it != floatMembers.end())
                {
                    it->second.get() = value;
                }
            }

            /**
             * @brief Set the Value object
             * 
             * @param key key of the value to set
             * @param value value to set
             */
            void setValue(std::string key, std::string value)
            {
                SET_UNORDERED_MAP_VALUE(stringMembers, key, value);
                return;
                if (auto it = stringMembers.find(key); it != stringMembers.end())
                {
                    it->second.get() = value;
                }
            }
    };

    class Config
    {
        public:
            Config(std::string configFile)
            {
                m_ConfigFile = configFile;
                LoadConfigFile();
            }

            /**
             * @brief Read the config file and set the values to the ConfigData object
             * 
             */
            void LoadConfigFile()
            {
                cv::FileStorage fs(m_ConfigFile, cv::FileStorage::READ);
                if(!fs.isOpened()) throw std::runtime_error(std::string(__FILE__) + " could not open file:" + m_ConfigFile);

                std::vector<std::string> modelFiles;
                fs["modelFiles"] >> modelFiles;
                m_ConfigData.setValue("modelFiles", modelFiles);

                std::vector<std::string> acitFiles;
                fs["acitFiles"] >> acitFiles;
                m_ConfigData.setValue("acitFiles", acitFiles);

                std::vector<std::vector<float>> groundTruthPoses;
                fs["groundTruthPoses"] >> groundTruthPoses;
                m_ConfigData.setValue("groundTruthPoses", groundTruthPoses);

                m_ConfigData.setValue("histOffset", (int)fs["histOffset"]);
                m_ConfigData.setValue("histRad", (int)fs["histRad"]);
                m_ConfigData.setValue("searchRad", (int)fs["searchRad"]);

                m_ConfigData.setValue("zn", (float)fs["zn"]);
                m_ConfigData.setValue("zf", (float)fs["zf"]);

                m_ConfigData.setValue("classifierModelPath", (std::string)fs["classifierModelPath"]);
                
                std::vector<std::string> classifierLabels;
                fs["classifierLabels"] >> classifierLabels;
                m_ConfigData.setValue("classifierLabels", classifierLabels);
                m_ConfigData.setValue("classifierImageSize", (int)fs["classifierImageSize"]);
                m_ConfigData.setValue("classifierImageChannels", (int)fs["classifierImageChannels"]);
                
                std::vector<float> classifierMean, classifierStd;
                fs["classifierMean"] >> classifierMean;
                fs["classifierStd"] >> classifierStd;
                m_ConfigData.setValue("classifierMean",classifierMean);
                m_ConfigData.setValue("classifierStd", classifierStd);

                return fs.release();
            }

            /**
             * @brief Print the config file to the console
             * 
             */
            void PrintConfigFile()
            {
                std::cout << "Config file loaded: " << m_ConfigFile << std::endl;
                std::cout << "Model files: " << std::endl;
                for (auto& modelFile : m_ConfigData.ModelFiles)
                {
                    std::cout << "\t" << modelFile << std::endl;
                }

                std::cout << "ACIT files: " << std::endl;
                for (auto& acitFile : m_ConfigData.AcitFiles)
                {
                    std::cout << "\t" << acitFile << std::endl;
                }

                std::cout << "Ground truth poses: " << std::endl;
                for (auto& groundTruthPose : m_ConfigData.GroundTruthPoses)
                {
                    std::cout << "\t";
                    for (auto& value : groundTruthPose)
                    {
                        std::cout << value << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << "Hist offset: " << m_ConfigData.HistOffset << std::endl;
                std::cout << "Hist rad: " << m_ConfigData.HistRad << std::endl;
                std::cout << "Search rad: " << m_ConfigData.SearchRad << std::endl;
                std::cout << "Zn: " << m_ConfigData.Zn << std::endl;
                std::cout << "Zf: " << m_ConfigData.Zf << std::endl;

                std::cout << "Classifier model path: " << m_ConfigData.ClassifierModelPath << std::endl;
                std::cout << "Classifier labels: " << std::endl;
                for (auto& classifierLabel : m_ConfigData.ClassifierLabels)
                {
                    std::cout << "\t" << classifierLabel << std::endl;
                }
                std::cout << "Classifier image size: " << m_ConfigData.ClassifierImageSize << std::endl;
                std::cout << "Classifier image channels: " << m_ConfigData.ClassifierImageChannels << std::endl;
                std::cout << "Classifier mean: " << std::endl;
                for (auto& classifierMean : m_ConfigData.ClassifierMean)
                {
                    std::cout << "\t" << classifierMean << std::endl;
                }
                std::cout << "Classifier std: " << std::endl;
                for (auto& classifierStd : m_ConfigData.ClassifierStd)
                {
                    std::cout << "\t" << classifierStd << std::endl;
                }
            }

            /**
             * @brief Dump the ConfigData to the config file in the disk
             * 
             */
            void DumpConfigFile()
            {
                cv::FileStorage fs(m_ConfigFile, cv::FileStorage::WRITE);
                if(!fs.isOpened()) throw std::runtime_error(std::string(__FILE__) + " could not open file:" + m_ConfigFile);
                fs << "zn" << m_ConfigData.Zn;
                fs << "zf" << m_ConfigData.Zf;

                fs << "histOffset" << m_ConfigData.HistOffset;
                fs << "histRad" << m_ConfigData.HistRad;
                fs << "searchRad" << m_ConfigData.SearchRad;

                fs << "classifierModelPath" << m_ConfigData.ClassifierModelPath;
                fs << "classifierLabels" << m_ConfigData.ClassifierLabels;
                fs << "classifierImageSize" << m_ConfigData.ClassifierImageSize;
                fs << "classifierImageChannels" << m_ConfigData.ClassifierImageChannels;
                fs << "classifierMean" << m_ConfigData.ClassifierMean;
                fs << "classifierStd" << m_ConfigData.ClassifierStd;

                fs << "groundTruthPoses" << m_ConfigData.GroundTruthPoses;
                fs << "modelFiles" << m_ConfigData.ModelFiles;

                fs << "acitFiles" << m_ConfigData.AcitFiles;

                fs.release();
            }

            /**
             * @brief Get the Config Data object. This object is const meaning that it cannot be modified. To modify the config file, use the write function.
             * 
             * 
             * @return ConfigData
             */
            ConfigData GetConfigData()
            {
                // Create a copy of the ConfigData object
                ConfigData configData = this->m_ConfigData;
                // Prefix the model files with the m_TToolRootPath
                for (auto& modelFile : configData.ModelFiles)
                {
                    modelFile = std::string(m_TToolRootPath) + "/" + modelFile;
                }
                // Prefix the acit files with the m_TToolRootPath
                for (auto& acitFile : configData.AcitFiles)
                {
                    acitFile = std::string(m_TToolRootPath) + "/" + acitFile;
                }
                // Prefix the classifier model path with the m_TToolRootPath
                configData.ClassifierModelPath = std::string(m_TToolRootPath) + "/" + configData.ClassifierModelPath;
                return configData;
            }

            /**
             * @brief Set the TToolRootPath
             * TToolRootPath is the path to the root of the ttool folder. It is used to prefix the model files, acit files and classifier model path.
             * 
             * @return std::string 
             */
            void SetTToolRootPath(std::string ttoolRootPath)
            {
                m_TToolRootPath = ttoolRootPath;
            }

            /**
             * @brief Get the TTool Root Path
             * 
             * @return std::string 
             */
            std::string GetTToolRootPath()
            {
                return m_TToolRootPath;
            }

            /**
             * @brief Write a value to the config file.
             * 
             * @tparam T type of the value to write
             * @param key key of the value to write
             * @param value value to write
             * @param dumpConfigFile whether to dump the config file to the disk
             */
            template<typename T>
            void write(std::string key, T value, bool dumpConfigFile = true)
            {
                m_ConfigData.setValue(key, value);
                DumpConfigFile();
            }

        private:
            std::string m_ConfigFile;
            ConfigData m_ConfigData;
            std::string m_TToolRootPath = "";
    };
}