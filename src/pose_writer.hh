#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/core.hpp>


class PoseWriter
{
    public:
        PoseWriter(const std::string& filename, const std::string &configFile, const std::vector<std::string> &modelFiles)
        {
            outputFile.open(filename);
            if (!outputFile.is_open())
            {
                std::cerr << "Cannot open the pose file";
                exit(1);
            }
            outputFile << "config_file: " << configFile << std::endl;
            outputFile << "model_files: " << std::endl;
            int objectID = 1;
            for (const auto& modelFile : modelFiles)
            {
                outputFile << "\tmodelID (" << objectID++ << ")" << modelFile << std::endl;
            }

        }
        ~PoseWriter()
        {
            outputFile.close();
        }

        void write(cv::Matx44f& pose, int objectID)
        {
            outputFile << "objectID: " << objectID << std::endl;
            outputFile << "pose: " << std::endl;
            outputFile << '\t' << pose(0, 0) << ' ' << pose(0, 1) << ' ' << pose(0, 2) << ' ' << pose(0, 3) << std::endl;
            outputFile << '\t' << pose(1, 0) << ' ' << pose(1, 1) << ' ' << pose(1, 2) << ' ' << pose(1, 3) << std::endl;
            outputFile << '\t' << pose(2, 0) << ' ' << pose(2, 1) << ' ' << pose(2, 2) << ' ' << pose(2, 3) << std::endl;
            outputFile << '\t' << pose(3, 0) << ' ' << pose(3, 1) << ' ' << pose(3, 2) << ' ' << pose(3, 3) << std::endl;
        }

    private:
        std::ofstream outputFile;
        std::vector<std::string> modelFiles;
};