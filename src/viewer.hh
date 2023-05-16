#pragma once

#include <filesystem>
#include <string>
#include <opencv2/core.hpp>

#include "camera.hh"
#include "event.hh"

class Model;

class Viewer
{
public:
	virtual void UpdateViewer(int save_index) = 0;
	void StartSavingImages(const std::filesystem::path &path);
	void StopSavingImages();

	void set_display_images(bool dispaly_images);

	void UpdateEvent(ttool::EventType event)
	{
		m_Event = event;
	}

	void DrawInterface(cv::Mat frame)
	{
		cv::Scalar color(25, 25, 125);

		cv::Point statusPosition(5, 40);
		if (m_Event == ttool::EventType::ReadyLiveCamera)
		{
			cv::putText(frame, "Tracking Stopped, press P to continue...", statusPosition, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
		}
		else if (m_Event == ttool::EventType::PoseInput)
		{
			cv::putText(frame, "Pose Input Mode, adjust with Keymaps, press P to start Tracking...", statusPosition, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
		}
		else if (m_Event == ttool::EventType::Tracking)
		{
			cv::putText(frame, "Tracking..., press q to stop", statusPosition, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
		}

		if (m_ShowKeymaps)
		{
			cv::putText(frame, "Keymaps:", cv::Point(5, 60), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			// Translation
			cv::putText(frame, "W: Move model up", cv::Point(5, 80), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "S: Move model down", cv::Point(5, 100), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "A: Move model left", cv::Point(5, 120), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "D: Move model right", cv::Point(5, 140), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "Q: Move model forward", cv::Point(5, 160), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "E: Move model backward", cv::Point(5, 180), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			// Rotation
			cv::putText(frame, "I: Rotate model up", cv::Point(5, 200), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "K: Rotate model down", cv::Point(5, 220), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "J: Rotate model left", cv::Point(5, 240), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "L: Rotate model right", cv::Point(5, 260), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "U: Rotate model clockwise", cv::Point(5, 280), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "O: Rotate model counterclockwise", cv::Point(5, 300), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			// Model Management
			cv::putText(frame, "UP: Change model", cv::Point(5, 320), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "R: Reset model to ground truth", cv::Point(5, 340), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "P: Set current model pose as initial", cv::Point(5, 360), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "Y: Set current model pose as initial, and save to config file", cv::Point(5, 380), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			// Tracking
			cv::putText(frame, "Q: Stop tracking (while tracking)", cv::Point(5, 400), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
			cv::putText(frame, "P: Start tracking (while not tracking)", cv::Point(5, 420), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
		}
		else
		{
			cv::putText(frame, "Press H to show keymaps", cv::Point(5, 60), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1);
		}
	}

	void ToggleShowKeymaps()
	{
		m_ShowKeymaps = !m_ShowKeymaps;
	}

protected:
	void PrintID(int fid, cv::Mat &frame);
	void ShowImage(const cv::Mat &frame);

	std::shared_ptr<Camera> camera_ptr_ = nullptr;
	std::string name_{};
	std::filesystem::path save_path_{};
	bool display_images_ = true;
	bool save_images_ = false;
	bool initialized_ = false;
	
	bool m_ShowKeymaps = false;
	ttool::EventType m_Event = ttool::EventType::None;
};

class ImageViewer : public Viewer
{
public:
	void Init(const std::string &name, std::shared_ptr<Camera> camera_ptr);
	ImageViewer() = default;

	void UpdateViewer(int save_index) override;
};

class View;

class ContourViewer : public Viewer
{
public:
	void Init(const std::string &name, View *view, const std::vector<std::shared_ptr<Model>> &objects, std::shared_ptr<Camera> camera);
	ContourViewer() = default;

	void UpdateViewer(int save_index) override;
	cv::Mat DrawContourOverlay(View *view, const std::vector<std::shared_ptr<Model>> &objects, const cv::Mat &frame);

protected:
	View *renderer_;
	std::vector<std::shared_ptr<Model>> objects_;
};

class FragmentViewer : public Viewer
{
public:
	void Init(const std::string &name, View *view, const std::vector<std::shared_ptr<Model>> &objects, std::shared_ptr<Camera> camera);
	FragmentViewer() = default;

	void UpdateViewer(int save_index) override;
	cv::Mat DrawFragmentOverlay(View *view, const std::vector<std::shared_ptr<Model>> &objects, const cv::Mat &frame);

protected:
	View *renderer_;
	std::vector<std::shared_ptr<Model>> objects_;
};