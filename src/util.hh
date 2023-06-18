#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <filesystem>
#include <string_view>


#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <glog/logging.h>

namespace tk {

inline bool IsFileExist (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

} // namespace tk

namespace ttool::standaloneUtils
{
    inline void createVideo(const std::filesystem::path& path)
    {
        std::string command = "ffmpeg -framerate 30 -i " + path.string() + "/%d.png -c:v libx264 -profile:v high -crf 5 -pix_fmt yuv420p " + path.filename().string() + ".mp4";
        std::cout << command << std::endl;
        system(command.c_str());
    
        command = "ffmpeg -framerate 30 -i " + path.string() + "/raw/%d.png -c:v libx264 -profile:v high -crf 5 -pix_fmt yuv420p " + path.filename().string() + "_raw.mp4";
        std::cout << command << std::endl;
        system(command.c_str());
    }

    inline void deleteImages(const std::filesystem::path& path)
    {
        std::string command = "rm -rf " + path.string();
        std::cout << command << std::endl;
        system(command.c_str());
    }

    inline void createVideoAndDeleteImages(const std::filesystem::path& path)
    {
        createVideo(path);
        deleteImages(path);
    }

    inline bool startsWith(const std::string& str, const std::string& prefix) {
        return str.compare(0, prefix.length(), prefix) == 0;
    }

    inline void makeVideoFromAllSavedImages(const std::string& directoryPath)
    {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d");
        std::string currentDate = ss.str();

        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_directory()) {
                    std::string folderName = entry.path().filename().string();
                    if (startsWith(folderName, currentDate))
                    {
                        std::cout << "Folder: " << entry.path().string() << std::endl;
                        createVideoAndDeleteImages(entry.path().string());
                    }
                }
            }
        } else {
            std::cout << "Directory not found." << std::endl;
        }
    }

}