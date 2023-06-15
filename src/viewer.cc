#include <opencv2/highgui.hpp>

#include "viewer.hh"
#include "view.hh"


using namespace ttool::standaloneUtils;

void Viewer::StartSavingImages(const std::filesystem::path &path)
{
	save_images_ = true;
	save_path_ = path;
}

void Viewer::StopSavingImages()
{
	save_images_ = false;
}

void Viewer::set_display_images(bool dispaly_images)
{
	display_images_ = dispaly_images;
}

void Viewer::PrintID(int fid, cv::Mat &frame)
{
	std::stringstream sstr;
	sstr << "#" << std::setw(3) << std::setfill('0') << fid;
	cv::putText(frame, sstr.str(), cv::Point(5, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(0, 255, 0), 1);
}

void Viewer::ShowImage(const cv::Mat &frame)
{
	if (frame.cols > 1440)
	{
		cv::Mat small_out;
		cv::resize(frame, small_out, cv::Size(frame.cols * 0.75, frame.rows * 0.75));
		cv::imshow(name_, small_out);
	}
	else
	{
		cv::imshow(name_, frame);
	}
}

void ImageViewer::Init(const std::string &name, std::shared_ptr<Camera> camera_ptr)
{
	camera_ptr_ = std::move(camera_ptr);
	name_ = name;
	initialized_ = true;
}

void ImageViewer::UpdateViewer(int save_index, int fps)
{
	auto frame = camera_ptr_->image();

	if (display_images_)
		ShowImage(frame);

	if (save_images_)
		cv::imwrite(
			save_path_.string() + name_ + "_" + std::to_string(save_index) + ".png",
			frame);
}

void UnifiedViewer::Init(const std::string &name, View *view, std::shared_ptr<Model> object, std::shared_ptr<Camera> camera_ptr)
{
	camera_ptr_ = std::move(camera_ptr);
	renderer_ = view;
	objects_ = object;
	name_ = name;
	initialized_ = true;
}

void UnifiedViewer::StopSavingImages()
{
	save_images_ = false;
	m_SaveIndex = 0;
}


void UnifiedViewer::UpdateViewer(int fid, int fps)
{
	const cv::Mat &frame = camera_ptr_->image();
	auto res_img = DrawOverlay(renderer_, objects_, frame);
	PrintID(fid, res_img);
	PrintFPS(fps, res_img);

	if (display_images_)
	{
		DrawInterface(res_img);
		ShowImage(res_img);
	}

	if (save_images_)
	{	
		cv::imwrite(save_path_.string() + "/raw/" + std::to_string(m_SaveIndex) + ".png", camera_ptr_->image());
		cv::imwrite(save_path_.string() + std::to_string(m_SaveIndex) + ".png", res_img);
		++m_SaveIndex;
	}
}

cv::Mat UnifiedViewer::DrawOverlay(View *view, std::shared_ptr<Model> object, const cv::Mat &frame)
{
	// render the models with phong shading
	view->setLevel(0);

	view->RenderShaded(object, GL_FILL, cv::Point3f(1.0, 0.5, 0.0), true);

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

	view->setLevel(0);
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