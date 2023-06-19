#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/core.hpp>


namespace ttool::standaloneUtils
{
    class PoseWriter
    {
        public:
            PoseWriter(){};
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

            void write(cv::Matx44f& pose, int objectID)
            {
                m_OutputFile << "objectID: " << objectID << std::endl;
                m_OutputFile << "pose: " << std::endl;
                m_OutputFile << '\t' << pose(0, 0) << ' ' << pose(0, 1) << ' ' << pose(0, 2) << ' ' << pose(0, 3) << std::endl;
                m_OutputFile << '\t' << pose(1, 0) << ' ' << pose(1, 1) << ' ' << pose(1, 2) << ' ' << pose(1, 3) << std::endl;
                m_OutputFile << '\t' << pose(2, 0) << ' ' << pose(2, 1) << ' ' << pose(2, 2) << ' ' << pose(2, 3) << std::endl;
                m_OutputFile << '\t' << pose(3, 0) << ' ' << pose(3, 1) << ' ' << pose(3, 2) << ' ' << pose(3, 3) << std::endl;
            }

        private:
            std::ofstream m_OutputFile;
            std::vector<std::string> m_ModelFiles;
    };
}