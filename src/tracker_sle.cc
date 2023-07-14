#include <iostream>
#include <fstream>
#include <opencv2/highgui.hpp>

#include "m_func.hh"
#include "histogram.hh"
#include "search_line.hh"
#include "tracker_sle.hh"

enum {
	RUN_TRACK = 0,
	RUN_DEBUG = 1,
};

SLETracker::SLETracker(const cv::Matx33f& K, std::shared_ptr<Object3D> objects)
	: SLTracker(K, objects)
{}

void SLETracker::ComputeJac(
	std::shared_ptr<Object3D> object,
	int m_id, 
	const cv::Mat& frame,
	const cv::Mat& depth_map,
	const cv::Mat& depth_inv_map, 
	cv::Matx66f& wJTJM, cv::Matx61f& JTM) 
{
	float* depth_data = (float*)depth_map.ptr<float>();
	float* depth_inv_data = (float*)depth_inv_map.ptr<float>();
	uchar* frame_data = frame.data;
	const std::vector<std::vector<cv::Point> >& search_points = search_line->search_points;
	const std::vector<std::vector<cv::Point2f> >& bundle_prob = search_line->bundle_prob;

	JTM = cv::Matx61f::zeros();
	wJTJM = cv::Matx66f::zeros();
	float* JT = JTM.val;
	float* wJTJ = wJTJM.val;

	float zf = view->GetZFar();
	float zn = view->GetZNear();
	cv::Matx33f K = view->GetCalibrationMatrix().get_minor<3, 3>(0, 0);
    auto K_inv = K.inv();
    auto K_inv_data = K_inv.val;
	float fx = K(0,0);
	float fy = K(1,1);

	for (int r = 0; r < search_points.size(); r++) {
		if (!search_line->actives[r])
			continue;

		int eid = search_points[r][search_points[r].size()-1].y;
		if (eid < 0)
			continue;

		int last = search_points[r].size()-2;
		float nx = search_points[r][last].x - search_points[r][0].x;
		float ny = search_points[r][last].y - search_points[r][0].y;
		float dnxy = 1.0f / sqrt(nx*nx + ny*ny);
		nx *= -dnxy;
		ny *= -dnxy;

		int mid = search_points[r][search_points[r].size() - 1].x;
		int mx = search_points[r][mid].x;
		int my = search_points[r][mid].y;
		int zidx = my * depth_map.cols + mx;
		float ex = nx * (search_points[r][mid].x - search_points[r][eid].x) + ny * (search_points[r][mid].y - search_points[r][eid].y);
		
		float we = tukey_weight(ex, 10) * ecweight(scores[r], 1.0f);

		float depth = 1.0f - depth_data[zidx];
		float D = 2.0f * zn * zf / (zf + zn - (2.0f * depth - 1.0) * (zf - zn));
		float Xc = D * (K_inv_data[0] * mx + K_inv_data[2]);
		float Yc = D * (K_inv_data[4] * my + K_inv_data[5]);
		float Zc = D;
		float Zc2 = Zc*Zc;

		float J[6];

		J[0] = nx * (-Xc*fx*Yc/Zc2) +     ny * (-fy -Yc*Yc*fy/Zc2);
		J[1] = nx * (fx + Xc*Xc*fx/Zc2) + ny * (Xc*Yc*fy/Zc2);
		J[2] = nx * (-fx*Yc/Zc)+          ny * (Xc*fy/Zc);
		J[3] = nx * (fx/Zc);
		J[4] =                            ny * (fy/Zc);
		J[5] = nx * (-Xc*fx/Zc2) +        ny * (-Yc*fy/Zc2);

		for (int n = 0; n < 6; n++) {
			JT[n] += we * ex * J[n];
		}

		for (int n = 0; n < 6; n++)
		for (int m = n; m < 6; m++) {
			wJTJ[n * 6 + m] += we * J[n] * J[m];
		}

		depth = 1.0f - depth_inv_data[zidx];
		D = 2.0f * zn * zf / (zf + zn - (2.0f * depth - 1.0) * (zf - zn));
		Xc = D * (K_inv_data[0] * mx + K_inv_data[2]);
		Yc = D * (K_inv_data[4] * my + K_inv_data[5]);
		Zc = D;
		Zc2 = Zc * Zc;

		J[0] = nx * (-Xc*fx*Yc/Zc2) +     ny * (-fy -Yc*Yc*fy/Zc2);
		J[1] = nx * (fx + Xc*Xc*fx/Zc2) + ny * (Xc*Yc*fy/Zc2);
		J[2] = nx * (-fx*Yc/Zc)+          ny * (Xc*fy/Zc);
		J[3] = nx * (fx/Zc);
		J[4] =                            ny * (fy/Zc);
		J[5] = nx * (-Xc*fx/Zc2) +        ny * (-Yc*fy/Zc2);

		for (int n = 0; n < 6; n++) {
			JT[n] += we * ex * J[n];
            if (std::isnan(JT[n])) {
                throw std::runtime_error("something is not right...");
            }
		}

		for (int n = 0; n < 6; n++)
		for (int m = n; m < 6; m++) {
			wJTJ[n * 6 + m] += we * J[n] * J[m];
		}
	}

	for (int i = 0; i < wJTJM.rows; i++)
	for (int j = i + 1; j < wJTJM.cols; j++) {
		wJTJM(j, i) = wJTJM(i, j);
	}
}

void SLETracker::FindMatchPointMaxProb(float diff) {
	std::vector<std::vector<cv::Point> >& search_points = search_line->search_points;
	const std::vector<std::vector<cv::Point2f> >& bundle_prob = search_line->bundle_prob;
	scores.resize(search_points.size());


	for (int r = 0; r < bundle_prob.size(); ++r) {
		float prob_max = 0.0f;
		search_points[r][search_points[r].size()-1].y = -1;
		scores[r] = 0.0f;

		int mid = search_points[r][search_points[r].size() - 1].x;

		float nx = search_line->norms[r].x;
		float ny = search_line->norms[r].y;

		for (int c = 3; c < bundle_prob[r].size()-3; ++c) {
			if (bundle_prob[r][c + 1].x - bundle_prob[r][c - 1].x > diff) {
				float prbf = 
					bundle_prob[r][c-3].x*
					bundle_prob[r][c-2].x*
					bundle_prob[r][c-1].x;
				float prbb = 
					bundle_prob[r][c-3].y*
					bundle_prob[r][c-2].y*
					bundle_prob[r][c-1].y;
				float prff = 
					bundle_prob[r][c+1].x*
					bundle_prob[r][c+2].x*
					bundle_prob[r][c+3].x;
				float prfb = 
					bundle_prob[r][c+1].y*
					bundle_prob[r][c+2].y*
					bundle_prob[r][c+3].y;

				float pr_C = prbb*prff;
				float pr_F = prbf*prff;
				float pr_B = prbb*prfb;

				// both tukey_weight(ex, 10) * slweight(pr_C, 1.0f); cam_regular 89.3
				if ((pr_C > pr_F) && (pr_C > pr_B)) {

					float ex = nx * (search_points[r][mid].x - search_points[r][c].x) + ny * (search_points[r][mid].y - search_points[r][c].y);
					float we = tukey_weight(ex, 10) * ecweight(pr_C, 1.0f);

					if (we > prob_max) {
						prob_max = we;
						scores[r] = pr_C;
						search_points[r][search_points[r].size()-1].y = c;
					}
				}
			}
		}
	}
}

void SLETracker::Track(std::vector<cv::Mat>& imagePyramid, std::shared_ptr<Object3D> object, int runs, cv::Matx44f& init_pose)
{
    for (int iter = 0; iter < runs * 4; iter++) {
        RunIteration(object, imagePyramid, 2, 12, 2, init_pose);
    }

    for (int iter = 0; iter < runs * 2; iter++) {
        RunIteration(object, imagePyramid, 1, 12, 2, init_pose);
    }

    for (int iter = 0; iter < runs * 1; iter++) {
        RunIteration(object, imagePyramid, 0, 12, 2, init_pose);
    }
}

void SLETracker::RunIteration(std::shared_ptr<Object3D> object, const std::vector<cv::Mat>& imagePyramid, int level, int sl_len, int sl_seg, cv::Matx44f& init_pose)
{
	m_trackingStatus.str(std::string()); // Clearing the string
	m_trackingStatus.precision(4);


	int width = view->GetWidth();
	int height = view->GetHeight();
	view->SetLevel(level);
	int numInitialized = 0;
	if (!object->isInitialized())
		return;

	numInitialized++;
	cv::Rect roi;
	roi = Compute2DROI(object, cv::Size(width / pow(2, level), height / pow(2, level)), 8);
	if (roi.area() == 0)
		return;



	while (roi.area() < 3000 && level > 0)
	{
		level--;
		view->SetLevel(level);
		roi = Compute2DROI(object, cv::Size(width / pow(2, level), height / pow(2, level)), 8);
	}

	view->SetLevel(level);
	view->RenderSilhouette(object, GL_FILL);
	cv::Mat depth_map = view->DownloadFrame(View::DEPTH);

	cv::Mat masks_map;
	if (numInitialized > 1) {
		masks_map = view->DownloadFrame(View::MASK);
	}	else {
		masks_map = depth_map;
	}

	if (!object->isInitialized())  
		return;

	roi = Compute2DROI(object, cv::Size(width / pow(2, level), height / pow(2, level)), 8);
	if (roi.area() == 0) {
		object->setPose(init_pose);
		m_trackingStatus << "Reset - due to ROI 0";
		return;
	}

	int m_id = (numInitialized <= 1) ? -1 : object->getModelID();
	cv::Mat mask_map;
	ConvertMask(masks_map, m_id, roi, mask_map);

	search_line->FindSearchLine(mask_map, imagePyramid[level], sl_len, sl_seg, true);

	if (numInitialized > 1) {
		FilterOccludedPoint(masks_map, depth_map);
	}

	GetBundleProb(imagePyramid[level]);

	FindMatchPointMaxProb(0.2);
	float avg = 0.0f;
	int cnt = 0;
	for (int i = 0; i < scores.size(); ++i) {
		if (search_line->actives[i]) {
			avg += scores[i];
			++cnt;
		}
	}
	avg /= cnt;

	view->RenderSilhouette(object, GL_FILL, true);
	cv::Mat depth_inv_map = view->DownloadFrame(View::DEPTH);
	cv::Matx66f wJTJ;
	cv::Matx61f JT;
	ComputeJac(object, m_id, imagePyramid[level], depth_map, depth_inv_map, wJTJ, JT);

	auto deltaT = Transformations::exp(-wJTJ.inv(cv::DECOMP_CHOLESKY) * JT);
	cv::Matx44f T_cm = deltaT * object->getPose();

	if (avg < 0.0125 / 2)
	{
		object->setPose(init_pose);
		m_trackingStatus << "Reset - " << avg;
	}
	else if (avg < 0.2 / 2)
	{
		m_trackingStatus << "Freezed - " << avg;
	}
	else
	{
		m_trackingStatus << "Tracking - " << avg;
		object->setPose(T_cm);
	}
}

void SLETracker::EstimatePoses(cv::Matx44f& init_pose, cv::Mat& frame) {
    std::vector<cv::Mat> imagePyramid;
    imagePyramid.push_back(frame);

    for (int l = 1; l < 4; l++) {
        cv::resize(frame, frame, cv::Size(frame.cols / pow(2, l), frame.rows / pow(2, l)));
        imagePyramid.push_back(frame);
    }

    if (initialized) {
        Track(imagePyramid, m_Object, 1, init_pose);
    }
}