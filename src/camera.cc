/**
 * SLET
 * Copyright (C) 2020  Hong Huang and Fan Zhong and Yuqing Sun and Xueying Qin (Shandong University)
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

#include "camera.hh"
#include "opencv2/calib3d.hpp"
#include "opencv2/opencv.hpp"

using namespace ttool::standaloneUtils;

Camera* Camera::BuildCamera(const std::string& frames)
{
	struct stat buf;

	Camera* grabber = NULL;
	if (0 != stat(frames.c_str(), &buf)) {
		grabber = new VideoLoaderCamera(2); // <-- TODO: set index as param, for external webcam (4), mounted webcam (2)
	} else if(S_ISDIR(buf.st_mode)) {
		grabber = new ImageLoaderCamera(frames);
	} else {
		grabber = new VideoLoaderCamera(frames);
	}

	if (-1 == grabber->Width) {
		delete grabber;
		return NULL;
	}	else {
		return grabber;
	}
}

Camera* Camera::BuildCamera(const int cameraId)
{
	struct stat buf;

	Camera* grabber = new VideoLoaderCamera(cameraId); // <-- TODO: set index as param, for external webcam (4), mounted webcam (2)

	if (-1 == grabber->Width) {
		delete grabber;
		return NULL;
	}	else {
		return grabber;
	}
}

const cv::Mat& Camera::Image() const { return m_Image; }

void Camera::Preprocess()
{
    cv::resize(m_Image, m_Image, cv::Size2i(m_PreprocessWidth, m_PreprocessHeight));
    std::vector<cv::Mat> undistMap;
    if( undistMap.size() == 0){
        undistMap.resize(2);
        cv::initUndistortRectifyMap(m_K, m_DistCoeff, cv::Mat(), cv::Mat(), m_Image.size(), CV_32FC1, undistMap[0], undistMap[1]);
    }
    cv::remap(m_Image, m_Image, undistMap[0], undistMap[1], cv::INTER_CUBIC);
}

void Camera::ReadFromFile(std::string filePath)
{
    cv::FileStorage fs(filePath, cv::FileStorage::READ);
    if(!fs.isOpened()) throw std::runtime_error(std::string(__FILE__)+" could not open file:"+filePath);

    int w = -1, h = -1;
    cv::Mat MCamera, MDist;
    fs["image_width"] >> w;
    fs["image_height"] >> h;
    fs["distortion_coefficients"] >> MDist;
    fs["camera_matrix"] >> MCamera;

    if (MCamera.cols == 0 || MCamera.rows == 0){
        fs["Camera_Matrix"] >> MCamera;
        if (MCamera.cols == 0 || MCamera.rows == 0)
            throw std::runtime_error(std::string(__FILE__)+" File :" + filePath + " does not contains valid camera matrix");
    }

    if (w == -1 || h == 0){
        fs["image_Width"] >> w;
        fs["image_Height"] >> h;
        if (w == -1 || h == 0)
           throw std::runtime_error(std::string(__FILE__)+  "File :" + filePath + " does not contains valid camera dimensions");
    }
    if (MCamera.type() != CV_32FC1)
        MCamera.convertTo(MCamera, CV_32FC1);
    SetK(MCamera);

    if (MDist.total() < 4){
        fs["Distortion_Coefficients"] >> MDist;
        if (MDist.total() < 4)
             throw std::runtime_error(std::string(__FILE__)+   "File :" + filePath + " does not contains valid distortion_coefficients" );
    }

    // convert to 32 and get the first elements only
    cv::Mat mdist32;
    MDist.convertTo(mdist32, CV_32FC1);

    cv::Mat Distorsion=cv::Mat::zeros(1, 5, CV_32FC1);
    for (int i = 0; i < mdist32.total(); i++)
        Distorsion.ptr<float>(0)[i] = mdist32.ptr<float>(0)[i];
    SetDistCoeff(MDist);

    SetPreprocessSize(w, h);

}