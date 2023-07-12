#pragma once

#include <filesystem>
#include <string>
#include <opencv2/core.hpp>

#include "camera.hh"
#include "event.hh"

using namespace ttool::standaloneUtils;

class Model;

class Viewer
{
public:
	/**
	 * @brief Update the viewer with the new frame and the current model
	 * 
	 * @param frameID current frame ID (used for saving images and printing it on the frame)
	 * @param fps     current fps (used for printing it on the frame)
	 */
	virtual void UpdateViewer(int frameID, int fps) = 0;

	/**
	 * @brief Set the Image Save Path object and toggle saving images, which save the current frame as an image to be processed as a video later
	 * 
	 * @param path 
	 */
	void StartSavingImages(const std::filesystem::path &path);
	/**
	 * @brief Stop saving images
	 * 
	 */
	void StopSavingImages();

	/**
	 * @brief toggle whether to display images on the viewer or not
	 * 
	 * @param displayImages 
	 */
	void SetDisplayImage(bool displayImages);

	/**
	 * @brief Update the event of the viualizer (i.e. the state of the program)
	 * 
	 * @param event 
	 */
	void UpdateEvent(ttool::EventType event) { m_Event = event; }

	/**
	 * @brief Draw the information on the frame, according to the current event (i.e. the state of the program)
	 * 
	 * @param frame 
	 */
	void DrawInterface(cv::Mat frame);

	/**
	 * @brief Toggle the show keymaps
	 * 
	 */
	void ToggleShowKeymaps() { m_ShowKeymaps = !m_ShowKeymaps; }

protected:
	/**
	 * @brief Print the current FPS on the frame
	 * 
	 * @param fps	fps
	 * @param frame frame to print the fps on
	 */
	void PrintFPS(int fps, cv::Mat &frame);

	/**
	 * @brief Print the current frame ID on the frame
	 * 
	 * @param fid	frame ID
	 * @param frame frame to print the frame ID on
	 */
	void PrintID(int fid, cv::Mat &frame);

	/**
	 * @brief Show the image on the viewer, which is the cv::imshow function, but we show the frame on to the same window everytime
	 * 
	 * @param frame 
	 */
	void ShowImage(const cv::Mat &frame);

	std::shared_ptr<ttool::standaloneUtils::Camera> m_CameraPtr = nullptr;
	std::string m_WindowName{};
	std::filesystem::path m_SavePath{};
	bool m_DisplayImages = true;
	bool m_SaveImages = false;
	bool m_IsInitialzed = false;

	bool m_ShowKeymaps = false;
	ttool::EventType m_Event = ttool::EventType::None;
};

class View;

/**
 * @brief Unified viewer shows the model and the cameraPtr on the same window
 * 
 */
class UnifiedViewer : public Viewer
{
public:
	/**
	 * @brief Initialize the viewer
	 * 
	 * @param name 		name of the window
	 * @param view 		view object to render the model
	 * @param object 	current model to be rendered
	 * @param cameraPtr camera pointer object to render the current frame
	 */
	void Init(const std::string &name, View *view, std::shared_ptr<Model> object, std::shared_ptr<Camera> cameraPtr);
	UnifiedViewer() = default;
	void StopSavingImages();

	void UpdateViewer(int frameID, int fps) override;

private:
	cv::Mat DrawOverlay(View *view, std::shared_ptr<Model> object, const cv::Mat &frame);

protected:
	View *renderer_;
	std::shared_ptr<Model> objects_;
	int m_SaveIndex = 0;
};