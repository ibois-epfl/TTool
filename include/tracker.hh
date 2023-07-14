#pragma once

#include <opencv2/core.hpp>

#include "object3d.hh"
#include "template_view.hh"
#include "camera.hh"

class Viewer;

class Tracker {
public:
	Tracker(const cv::Matx33f& K, std::shared_ptr<Object3D> objects);
	void Init() {}

	static Tracker* GetTracker(int id, const cv::Matx33f& K, const cv::Matx14f& distCoeffs, std::shared_ptr<Object3D> objects);

	virtual void ToggleTracking(int objectIndex, bool undistortedFrame);
	virtual void EstimatePoses(cv::Matx44f& init_pose, cv::Mat& frame) = 0;
    virtual void PreProcess(cv::Mat frame) {}
	virtual void PostProcess(cv::Mat frame) {}

	void Reset();

	std::string GetTrackingStatus() const { return m_trackingStatus.str(); };

protected:
	virtual void Track(std::vector<cv::Mat>& imagePyramid, std::shared_ptr<Object3D> objects, int runs, cv::Matx44f& initialPose) = 0;

	cv::Rect Compute2DROI(std::shared_ptr<Object3D> object, const cv::Size& maxSize, int offset);
	static cv::Rect computeBoundingBox(const std::vector<cv::Point3i>& centersIDs, int offset, int level, const cv::Size& maxSize);

	static void ConvertMask(const cv::Mat& maskm, uchar oid, cv::Mat& mask);
	static void ConvertMask(const cv::Mat& maskm, uchar oid, cv::Rect& roi, cv::Mat& mask);
	static void ShowMask(const cv::Mat& masks, cv::Mat& buf);

protected:
	std::shared_ptr<Object3D> m_Object;
	
	View* view;
	cv::Matx33f K;

	bool initialized;

	std::stringstream m_trackingStatus;
};

class Histogram;

class TrackerBase : public Tracker {
public:
	TrackerBase(const cv::Matx33f& K, std::shared_ptr<Object3D> objects);

	virtual void PreProcess(cv::Mat frame) override;
	virtual void PostProcess(cv::Mat frame) override;
	virtual void UpdateHist(cv::Mat frame);

protected:
	void DetectEdge(const cv::Mat& img, cv::Mat& edge_map);
	
protected:
	Histogram* hists;
};

class SearchLine;

class SLTracker: public TrackerBase {
public:
	SLTracker(const cv::Matx33f& K, std::shared_ptr<Object3D> objects);

	void GetBundleProb(const cv::Mat& frame);
	void FilterOccludedPoint(const cv::Mat& mask, const cv::Mat& depth);

protected:
	std::shared_ptr<SearchLine> search_line;
	std::vector<float> scores;
};

inline float GetDistance(const cv::Point& p1, const cv::Point& p2) {
	float dx = float(p1.x - p2.x);
	float dy = float(p1.y - p2.y);
	return sqrt(dx*dx + dy*dy);
}

/**
 *  This class extends the OpenCV ParallelLoopBody for efficiently parallelized
 *  computations. Within the corresponding for loop, the RGB values per pixel
 *  of a color input image are converted to their corresponding histogram bin
 *  index.
 */
class Parallel_For_convertToBins : public cv::ParallelLoopBody
{
private:
	cv::Mat _frame;
	cv::Mat _binned;

	uchar* frameData;
	int* binnedData;

	int _numBins;

	int _binShift;

	int _threads;

public:
	Parallel_For_convertToBins(const cv::Mat& frame, cv::Mat& binned, int numBins, int threads)
	{
		_frame = frame;

		binned.create(_frame.rows, _frame.cols, CV_32SC1);
		_binned = binned;

		frameData = _frame.data;
		binnedData = (int*)_binned.ptr<int>();

		_numBins = numBins;

		_binShift = 8 - log(numBins) / log(2);

		_threads = threads;
	}

	virtual void operator()(const cv::Range& r) const
	{
		int range = _frame.rows / _threads;

		int yEnd = r.end * range;
		if (r.end == _threads)
		{
			yEnd = _frame.rows;
		}

		for (int y = r.start * range; y < yEnd; y++)
		{
			uchar* frameRow = frameData + y * _frame.cols * 3;
			int* binnedRow = binnedData + y * _binned.cols;

			int idx = 0;
			for (int x = 0; x < _frame.cols; x++, idx += 3)
			{
				int ru = (frameRow[idx] >> _binShift);
				int gu = (frameRow[idx + 1] >> _binShift);
				int bu = (frameRow[idx + 2] >> _binShift);

				int binIdx = (ru * _numBins + gu) * _numBins + bu;

				binnedRow[x] = binIdx;
			}
		}
	}
};