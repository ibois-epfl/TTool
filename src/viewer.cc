#include <opencv2/highgui.hpp>

#include "viewer.hh"
#include "view.hh"


using namespace ttool::standaloneUtils;

void Viewer::StartSavingImages(const std::filesystem::path &path)
{
	m_SaveImages = true;
	m_SavePath = path;
}

void Viewer::StopSavingImages()
{
	m_SaveImages = false;
}

void Viewer::SetDisplayImage(bool dispalyImages)
{
	m_DisplayImages = dispalyImages;
}

void Viewer::DrawInterface(cv::Mat frame)
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

void Viewer::PrintID(int fid, cv::Mat &frame)
{
	std::stringstream sstr;
	sstr << "#" << std::setw(3) << std::setfill('0') << fid;
	cv::putText(frame, sstr.str(), cv::Point(5, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(0, 255, 0), 1);
}

void Viewer::PrintFPS(int fps, cv::Mat &frame)
{
	std::stringstream sstr;
	sstr << "FPS" << std::setw(3) << std::setfill('0') << fps;
	cv::putText(frame, sstr.str(), cv::Point(frame.size[1] - 150, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(0, 255, 0), 1);
}

void Viewer::ShowImage(const cv::Mat &frame)
{
	if (frame.cols > 1440)
	{
		cv::Mat small_out;
		cv::resize(frame, small_out, cv::Size(frame.cols * 0.75, frame.rows * 0.75));
		cv::imshow(m_WindowName, small_out);
	}
	else
	{
		cv::imshow(m_WindowName, frame);
	}
}

void UnifiedViewer::Init(const std::string &name, View *view, std::shared_ptr<Model> object, std::shared_ptr<Camera> camera_ptr)
{
	m_CameraPtr = std::move(camera_ptr);
	renderer_ = view;
	objects_ = object;
	m_WindowName = name;
	m_IsInitialzed = true;
}

void UnifiedViewer::StopSavingImages()
{
	m_SaveImages = false;
	m_SaveIndex = 0;
}


void UnifiedViewer::UpdateViewer(int frameID, int fps)
{
	const cv::Mat &frame = m_CameraPtr->Image();
	auto res_img = DrawOverlay(renderer_, objects_, frame);
	PrintID(frameID, res_img);
	PrintFPS(fps, res_img);

	if (m_DisplayImages)
	{
		DrawInterface(res_img);
		ShowImage(res_img);
	}

	if (m_SaveImages)
	{	
		cv::imwrite(m_SavePath.string() + "/raw/" + std::to_string(m_SaveIndex) + ".png", m_CameraPtr->Image());
		cv::imwrite(m_SavePath.string() + std::to_string(m_SaveIndex) + ".png", res_img);
		++m_SaveIndex;
	}
}

cv::Mat UnifiedViewer::DrawOverlay(View *view, std::shared_ptr<Model> object, const cv::Mat &frame)
{
	// render the models with phong shading
	view->SetLevel(0);

	view->RenderShaded(object, GL_FILL, cv::Point3f(1.0, 0.5, 0.0));

	// download the rendering to the CPU
	cv::Mat rendering = view->DownloadFrame(View::RGB);

	// download the depth buffer to the CPU
	cv::Mat depth = view->DownloadFrame(View::DEPTH);

	// compose the rendering with the current camera image for demo purposes (can be done more efficiently directly in OpenGL)
	float alpha = 0.5f;
	cv::Mat result = frame.clone();
	for (int y = 0; y < frame.rows; y++)
		for (int x = 0; x < frame.cols; x++)
		{
			cv::Vec3b color = rendering.at<cv::Vec3b>(y, x);
			if (depth.at<float>(y, x) != 0.0f)
			{
				result.at<cv::Vec3b>(y, x)[0] = alpha * color[2] + (1.0f - alpha) * frame.at<cv::Vec3b>(y, x)[2];
				result.at<cv::Vec3b>(y, x)[1] = alpha * color[1] + (1.0f - alpha) * frame.at<cv::Vec3b>(y, x)[1];
				result.at<cv::Vec3b>(y, x)[2] = alpha * color[0] + (1.0f - alpha) * frame.at<cv::Vec3b>(y, x)[0];
			}
		}

	view->SetLevel(0);
	view->RenderSilhouette(object, GL_FILL);

	cv::Mat depth_map = view->DownloadFrame(View::DEPTH);
	cv::Mat masks_map;
	masks_map = depth_map;

	cv::Mat mask_map;
	view->ConvertMask(masks_map, mask_map, object->getModelID());

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask_map, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	cv::Vec3b color;
	color = cv::Vec3b(0, 255, 0);

	for (auto contour : contours)
		for (auto pt : contour)
		{
			result.at<cv::Vec3b>(pt) = color;
		}

	return result;
}