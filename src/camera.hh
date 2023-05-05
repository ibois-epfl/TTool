#pragma once

#include <climits>
#include <string>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
// #include <glog/logging.h>

#ifdef WIN32
#include "base/dirent_win.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

class VideoLoaderCamera;
class Camera {
public:
	static Camera* BuildCamera(const std::string& frames);
	static Camera* BuildCamera(int cameraID);

	virtual bool UpdateCamera() = 0;

	const cv::Mat& image() const;

    void preprocess();

	void SetK(cv::Matx33f K) {
		m_K = K;
	}

	cv::Matx33f GetK() {
		return m_K;
	}

	void SetDistCoeff(std::vector<float> distCoeff) {
		m_distCoeff = distCoeff;
	}

	void SetPreprocessSize(int width, int height) {
		m_preprocessWidth = width;
		m_preprocessHeight = height;
	}

	int width = -1;
	int height = -1;
protected:
	cv::Mat image_;
	int frame_index_ = 0;

	cv::Matx33f m_K;
	std::vector<float> m_distCoeff;
	int m_preprocessWidth;
	int m_preprocessHeight;
};

class VideoLoaderCamera: public Camera {
public:
	VideoLoaderCamera(const std::string& frames) {
		if (cap_.open(frames)) {
			width = (int)cap_.get(cv::CAP_PROP_FRAME_WIDTH);
			height = (int)cap_.get(cv::CAP_PROP_FRAME_HEIGHT);
		}
	}

	VideoLoaderCamera(int cam_id) {
		if (cap_.open(cam_id)) {
			width = (int)cap_.get(cv::CAP_PROP_FRAME_WIDTH);
			height = (int)cap_.get(cv::CAP_PROP_FRAME_HEIGHT);
		}
	}

	virtual bool UpdateCamera() override {
		bool ret = false;
		ret = cap_.read(image_);
		preprocess();
		frame_index_++;
		return ret;
	}

	cv::VideoCapture cap_;
};

static int only_png(const struct dirent *entry) {
	std::string fname(entry->d_name);
	int len = fname.length();
	if (len < 4) {
		return 0;
	}

	int pass;
	std::string dot_ext(fname.substr(len - 4, 4));
	if (dot_ext == std::string(".png")) {
		pass = 1;
	}	else {
		pass = 0;
	}

	return pass;
}

static int AlphaSort(const void *a, const void *b) {
	struct dirent **pa = (struct dirent **)a;
	struct dirent **pb = (struct dirent **)b;

	return strcoll((*pa)->d_name, (*pb)->d_name);
}

class ImageLoaderCamera: public Camera{
public:
	ImageLoaderCamera(const std::string& image_dir) {
		image_dir_ = image_dir;
		frame_count_ = scandir(image_dir.c_str(), &files, only_png, alphasort);

		std::stringstream stm;
		stm << image_dir << '/' << files[0]->d_name;
		cv::Mat frame = cv::imread(stm.str());
		if (!frame.empty()) {
			width = frame.cols;
			height = frame.rows;
		}
	}

	~ImageLoaderCamera() {
		for (int i = 0; i < frame_count_; i++) {
			free (files[i]);
		}
		free (files);
	}

	virtual bool UpdateCamera() override {
		if (frame_index_ == frame_count_)
			return false;

		std::stringstream stm;
		stm << image_dir_ << '/' << files[frame_index_]->d_name;
		image_ = cv::imread(stm.str());
        preprocess();
		if (image_.empty())
			return false;

		frame_index_++;
		return true;
	}

	std::string image_dir_;
	struct dirent **files;
	int frame_count_;
};