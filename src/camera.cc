#include "camera.hh"
#include "opencv2/calib3d.hpp"
#include "opencv2/opencv.hpp"

Camera* Camera::BuildCamera(const std::string& frames) {
	//struct _stat buf;
	struct stat buf;

	Camera* grabber = NULL;
	if (0 != stat(frames.c_str(), &buf)) {
        std::cout << "Openning VideoLoaderCamera(1)\n";
		grabber = new VideoLoaderCamera(2); // <-- TODO: set index as param, for external webcam (4), mounted webcam (2)
	} else if(S_ISDIR(buf.st_mode)) {
		grabber = new ImageLoaderCamera(frames);
	} else {
		grabber = new VideoLoaderCamera(frames);
	}

	if (-1 == grabber->width) {
		delete grabber;
		return NULL;
	}	else {
		return grabber;
	}
}

Camera* Camera::BuildCamera(const int cameraId) {
	struct stat buf;

	Camera* grabber = new VideoLoaderCamera(cameraId); // <-- TODO: set index as param, for external webcam (4), mounted webcam (2)

	if (-1 == grabber->width) {
		delete grabber;
		return NULL;
	}	else {
		return grabber;
	}
}

const cv::Mat& Camera::image() const { return image_; }

void Camera::preprocess() {
    cv::resize(image_, image_, cv::Size2i(m_preprocessWidth, m_preprocessHeight));
    std::vector<cv::Mat> undistMap;
    if( undistMap.size() == 0){
        undistMap.resize(2);
        cv::initUndistortRectifyMap(m_K, m_distCoeff, cv::Mat(), cv::Mat(), image_.size(), CV_32FC1, undistMap[0], undistMap[1]);
    }
    cv::remap(image_, image_, undistMap[0], undistMap[1], cv::INTER_CUBIC);
}