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

            // Unordered maps are used to help Setters more dynamic typed
            std::unordered_map<std::string, std::reference_wrapper<std::vector<std::string>>> stringVectorMembers = {
                {"modelFiles", std::ref(ModelFiles)},
                {"acitFiles", std::ref(AcitFiles)}
            };

            std::unordered_map<std::string, std::reference_wrapper<std::vector<std::vector<float>>>> floatVectorVectorMembers = {
                {"groundTruthPoses", std::ref(GroundTruthPoses)}
            };

            std::unordered_map<std::string, std::reference_wrapper<int>> intMembers = {
                {"histOffset", std::ref(HistOffset)},
                {"histRad", std::ref(HistRad)},
                {"searchRad", std::ref(SearchRad)}
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
                // Prefix the model files with the __TTOOL_ROOT_PATH__ macro
                for (auto& modelFile : configData.ModelFiles)
                {
                    modelFile = std::string(__TTOOL_ROOT_PATH__) + "/" + modelFile;
                }
                // Prefix the acit files with the __TTOOL_ROOT_PATH__ macro
                for (auto& acitFile : configData.AcitFiles)
                {
                    acitFile = std::string(__TTOOL_ROOT_PATH__) + "/" + acitFile;
                }
                // Prefix the classifier model path with the __TTOOL_ROOT_PATH__ macro
                configData.ClassifierModelPath = std::string(__TTOOL_ROOT_PATH__) + "/" + configData.ClassifierModelPath;
                return configData;
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
    };
}