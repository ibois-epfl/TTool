#pragma once

#include "tracker.hh"

class SLETracker: public SLTracker {
public:
	SLETracker(const cv::Matx33f& K, std::vector<std::shared_ptr<Object3D>>& objects);
    void EstimatePoses(cv::Matx44f& init_pose);

protected:
	void Track(std::vector<cv::Mat>& imagePyramid, std::vector<std::shared_ptr<Object3D>>& objects, int runs = 1) override;
    void Track(std::vector<cv::Mat>& imagePyramid, std::vector<std::shared_ptr<Object3D>>& objects, int runs, cv::Matx44f& init_pose);


	void RunIteration(std::vector<std::shared_ptr<Object3D>>& objects, const std::vector<cv::Mat>& imagePyramid, int level, int sl_len, int sl_seg, int run_type = 0);
    void RunIteration(std::vector<std::shared_ptr<Object3D>>& objects, const std::vector<cv::Mat>& imagePyramid, int level, int sl_len, int sl_seg, cv::Matx44f& init_pose);
	void ComputeJac(std::shared_ptr<Object3D> object, int m_id, const cv::Mat& frame, const cv::Mat& depth_map, const cv::Mat& depth_inv_map, cv::Matx66f& wJTJM, cv::Matx61f& JTM);
	void FindMatchPointMaxProb(float diff);

protected:
};