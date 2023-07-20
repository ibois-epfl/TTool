#pragma once

#include <climits>
#include <string>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>

#ifdef WIN32
#include "base/dirent_win.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

namespace ttool::standaloneUtils
{
	class VideoLoaderCamera;
	class Camera
	{
	public:
		/**
		 * @brief Construct a new Camera object from a video file or a directory of images
		 * 
		 * @param frames 
		 */
		static Camera* BuildCamera(const std::string& frames);

		/**
		 * @brief Construct a new Camera object from a webcam
		 * 
		 * @param cameraID 
		 */
		static Camera* BuildCamera(int cameraID);

		/**
		 * @brief Update the camera and return true if a new frame is available
		 * 
		 * @return true 
		 * @return false 
		 */
		virtual bool UpdateCamera() = 0;

		/**
		 * @brief Get the current frame image
		 * 
		 * @return const cv::Mat& 
		 */
		const cv::Mat& Image() const;

		/**
		 * @brief Intialize camera parameters from a file
		 * 
		 * @param filePath is a path to the calibration YAML file
		 */
		void ReadFromFile(std::string filePath);

	public: // Getters and Setters
		void SetK(cv::Matx33f K) { m_K = K; }

		cv::Matx33f GetK() { return m_K; }

		void SetDistCoeff(std::vector<float> distCoeff) { m_DistCoeff = distCoeff; }

		void SetPreprocessSize(int width, int height) { m_PreprocessWidth = width; m_PreprocessHeight = height; }

		int GetPreprocessWidth() { return m_PreprocessWidth; }

		int GetPreprocessHeight() { return m_PreprocessHeight; }

	protected:
		void Preprocess();

	public:
		int Width = -1;
		int Height = -1;

	protected:
		int m_FrameIndex = 0;
		cv::Mat m_Image;
		cv::Matx33f m_K;
		std::vector<float> m_DistCoeff;
		int m_PreprocessWidth;
		int m_PreprocessHeight;
	};

	class VideoLoaderCamera: public Camera {
	public:
		VideoLoaderCamera(const std::string& frames)
		{
			if (Capture.open(frames))
			{
				Width = (int)Capture.get(cv::CAP_PROP_FRAME_WIDTH);
				Height = (int)Capture.get(cv::CAP_PROP_FRAME_HEIGHT);
			}
		}

		VideoLoaderCamera(int cam_id)
		{
			if (Capture.open(cam_id))
			{
				Width = (int)Capture.get(cv::CAP_PROP_FRAME_WIDTH);
				Height = (int)Capture.get(cv::CAP_PROP_FRAME_HEIGHT);
			}
		}

		virtual bool UpdateCamera() override {
			bool ret = false;
			ret = Capture.read(m_Image);
			Preprocess();
			m_FrameIndex++;
			return ret;
		}

		cv::VideoCapture Capture;
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

	class ImageLoaderCamera: public Camera
	{
	public:
		ImageLoaderCamera(const std::string& image_dir)
		{
			ImageDir = image_dir;
			FrameCount = scandir(image_dir.c_str(), &Files, only_png, alphasort);

			std::stringstream stm;
			stm << image_dir << '/' << Files[0]->d_name;
			cv::Mat frame = cv::imread(stm.str());
			if (!frame.empty())
			{
				Width = frame.cols;
				Height = frame.rows;
			}
		}

		~ImageLoaderCamera()
		{
			for (int i = 0; i < FrameCount; i++) {
				free (Files[i]);
			}
			free (Files);
		}

		virtual bool UpdateCamera() override
		{
			if (m_FrameIndex == FrameCount)
				return false;

			std::stringstream stm;
			stm << ImageDir << '/' << Files[m_FrameIndex]->d_name;
			m_Image = cv::imread(stm.str());
			Preprocess();
			if (m_Image.empty())
				return false;

			m_FrameIndex++;
			return true;
		}

		std::string ImageDir;
		struct dirent **Files;
		int FrameCount;
	};
}
