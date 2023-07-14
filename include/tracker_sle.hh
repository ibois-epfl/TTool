#pragma once

#include "tracker.hh"

class SLETracker: public SLTracker {
public:
	SLETracker(const cv::Matx33f& K, std::shared_ptr<Object3D> objects);
	void EstimatePoses(cv::Matx44f& initialPose, cv::Mat& frame);

protected:
    virtual void Track(std::vector<cv::Mat>& imagePyramid, std::shared_ptr<Object3D> objects, int runs, cv::Matx44f& initialPose) override;

    void RunIteration(std::shared_ptr<Object3D> objects, const std::vector<cv::Mat>& imagePyramid, cv::Matx44f& initialPose);
	void RunIteration(std::shared_ptr<Object3D> objects, const std::vector<cv::Mat>& imagePyramid, int level, int sl_len, int sl_seg, cv::Matx44f& initialPose);
	void ComputeJac(std::shared_ptr<Object3D> object, int m_id, const cv::Mat& frame, const cv::Mat& depth_map, const cv::Mat& depth_inv_map, cv::Matx66f& wJTJM, cv::Matx61f& JTM);
	void FindMatchPointMaxProb(float diff);

protected:
};