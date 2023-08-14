#pragma once

#define GLFW_INCLUDE_NONE
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <filesystem>
#include <string_view>
#include <glm/glm.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <glog/logging.h>

#include <stdlib.h>
#include <stdio.h>


namespace ttool::gl
{
    /**
     * @brief Convert a cv::Mat to glm::mat4
     * 
     * @param cvMat 
     * @param glmMat 
     */
    inline void CvtCvMat2GlmMat(const cv::Mat &cvMat, glm::mat4 &glmMat)
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                glmMat[j][i] = cvMat.at<float>(i, j);
            }
        }
    }
}

namespace ttool::standaloneUtils
{
    /**
     * @brief Callback function for GLFW error
     * 
     * @param error 
     * @param description 
     */
    inline void error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    /**
     * @brief Initialize GLFW and GLEW
     * 
     * @return GLFWwindow* 
     */
    inline static GLFWwindow* InitializeStandalone()
    {
        // GLEW initialization
        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
            exit(EXIT_FAILURE);
        
        GLFWwindow* m_GLFWWindow;
        const char* m_GlslVersion;
        bool m_IsWindowOpen;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, false);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // the window will stay hidden after creation
        m_GLFWWindow = glfwCreateWindow(640, 480, "", NULL, NULL);
        if (m_GLFWWindow == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(m_GLFWWindow);
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            std::cout << "Failed to initialize GLEW" << std::endl;
            exit(EXIT_FAILURE);
        }

        return m_GLFWWindow;
    }

    /**
     * @brief Terminate GLFW
     * 
     * @param m_GLFWWindow 
     */
    inline static void TerminateStandalone(GLFWwindow* m_GLFWWindow)
    {
        glfwDestroyWindow(m_GLFWWindow);
        glfwTerminate();
    }
}
namespace tk
{
    inline bool IsFileExist (const std::string& name)
    {
        std::ifstream f(name.c_str());
        return f.good();
    }
}

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