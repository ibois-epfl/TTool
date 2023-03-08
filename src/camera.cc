#include "camera.hh"
#include "opencv2/calib3d.hpp"
#include "opencv2/opencv.hpp"

Camera* Camera::BuildCamera(const std::string& frames) {
	//struct _stat buf;
	struct stat buf;

	Camera* grabber = NULL;
	//if (0 != _stat(frames.c_str(), &buf)) {
	if (0 != stat(frames.c_str(), &buf)) {
        std::cout << "Openning VideoLoaderCamera(1)\n";
		grabber = new VideoLoaderCamera(0); // <-- TODO: set index as param, for external webcam (4), mounted webcam (2)
	//} else if(_S_IFDIR & buf.st_mode) {
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

const cv::Mat& Camera::image() const { return image_; }

void Camera::preprocess() {
    return;
    cv::resize(image_, image_, cv::Size2i(1280, 720));
    width = 1280;
    height = 720;
    auto K = cv::Matx33f(5.6160752462847097e+02, 0., 6.4138172226013501e+02, 0.,
                         5.6291538638344332e+02, 3.4156306804983251e+02, 0., 0., 1.);
    auto distCoeffs = cv::Matx<float, 1, 5>(-2.5303888172190003e-01,8.3509753739103201e-02,
                                            3.7774747611573724e-05, 2.2045706561633034e-04,
                                            -1.3650724983076233e-02);
    std::vector<cv::Mat> undistMap;
    if( undistMap.size()==0){
        undistMap.resize(2);
        cv::initUndistortRectifyMap(K,distCoeffs,cv::Mat(),cv::Mat(),image_.size(),CV_32FC1,undistMap[0],undistMap[1]);
    }
    cv::remap(image_,image_,undistMap[0],undistMap[1],cv::INTER_CUBIC);

//     cv::Mat fgMask = cv::imread("./masks/drill/mask.jpg", cv::IMREAD_GRAYSCALE);
// //    cv::Mat fgMask = cv::imread("./masks/notsharp/UndistortedMaskSharper.jpg", cv::IMREAD_GRAYSCALE);
//     cv::threshold(fgMask, fgMask, 200, 255, fgMask.type());
//     cv::resize(fgMask, fgMask, cv::Size2i(width, height));
//     cv::Mat maskedFrame(image_.size(), image_.type(), cv::Scalar(0, 0, 0));
//     cv::cvtColor(fgMask, image_, cv::COLOR_GRAY2RGB);
}