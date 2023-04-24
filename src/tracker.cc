#include <iomanip>
#include <glog/logging.h>
#include <opencv2/highgui.hpp>
#include "view.hh"
#include "histogram.hh"
#include "tracker.hh"
#include "global_param.hh"
#include "object3d.hh"
#include "search_line.hh"
#include "tracker_sle.hh"
#include "viewer.hh"

Tracker::Tracker(const cv::Matx33f& K, std::vector<std::shared_ptr<Object3D>>& objects) {
	initialized = false;

	view = View::Instance();

	this->K = K;

	for (int i = 0; i < objects.size(); i++) {
		if (objects[i]->getModelID() == 0) {
			objects[i]->setModelID(i + 1);
		}
		this->objects.push_back(objects[i]);
		this->objects[i]->initBuffers();
		// Maybe this should be move to the run_on_video where the histogram is generated, otherwise segmentation fault
		// this->objects[i]->generateTemplates();
		this->objects[i]->reset();
	}
}

Tracker* Tracker::GetTracker(int id, const cv::Matx33f& K, const cv::Matx14f& distCoeffs, std::vector<std::shared_ptr<Object3D>>& objects) {
	Tracker* poseEstimator = NULL;

	tk::GlobalParam* gp = tk::GlobalParam::Instance();

	poseEstimator = new SLETracker(K, objects);

	CHECK(poseEstimator) << "Check |tracker_mode| in yml file";
	return poseEstimator;
}

void Tracker::reset() {
	for (auto & object : objects) {
		object->reset();
	}

	initialized = false;
}

void Tracker::AddViewer(std::shared_ptr<Viewer> viewer_ptr) {
	viewer_ptrs_.push_back(std::move(viewer_ptr));
}

bool Tracker::UpdateViewers(int iteration) {
	if (!viewer_ptrs_.empty()) {
		for (auto& viewer_ptr : viewer_ptrs_) {
			viewer_ptr->UpdateViewer(iteration);
		}
	}
	return true;
}

//std::vector<std::vector<cv::Point> > contours;
//cv::findContours(mask_map, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//std::vector<cv::Point3f> pts3d;
//cv::Matx44f pose = objects[objectIndex]->getPoseMatrix();
//cv::Matx44f normalization = objects[objectIndex]->getNormalization();
//view->BackProjectPoints(contours[0], depth_map, pose* normalization, pts3d);
//OutputPly(pts3d, "pts3d.ply");

void Tracker::ToggleTracking(int objectIndex, bool undistortedFrame) {
	if (objectIndex >= objects.size())
		return;

	//if (undistortedFrame) {
	//	cv::remap(frame, frame, map1, map2, cv::INTER_LINEAR);
	//}

	if (!objects[objectIndex]->isInitialized()) {

		objects[objectIndex]->initialize();
		initialized = true;
	}
	else {
		objects[objectIndex]->reset();

		initialized = false;
		for (int o = 0; o < objects.size(); o++) {
			initialized |= objects[o]->isInitialized();
		}
	}
}

void Tracker::EstimatePoses() {}

cv::Rect Tracker::Compute2DROI(std::shared_ptr<Object3D> object, const cv::Size& maxSize, int offset) {
	// PROJECT THE 3D BOUNDING BOX AS 2D ROI
	cv::Rect boundingRect;
	std::vector<cv::Point2f> projections;

	view->ProjectBoundingBox(object, projections, boundingRect);

    // Out of bound
	if (boundingRect.x >= maxSize.width || boundingRect.y >= maxSize.height || boundingRect.x + boundingRect.width <= 0 || boundingRect.y + boundingRect.height <= 0) {
		return {0, 0, 0, 0};
	}

	// CROP THE ROI AROUND THE SILHOUETTE
	cv::Rect roi = cv::Rect(boundingRect.x - offset, boundingRect.y - offset, boundingRect.width + 2 * offset, boundingRect.height + 2 * offset);

	if (roi.x < 0) {
		roi.width += roi.x;
		roi.x = 0;
	}

	if (roi.y < 0) {
		roi.height += roi.y;
		roi.y = 0;
	}

	if (roi.x + roi.width > maxSize.width) roi.width = maxSize.width - roi.x;
	if (roi.y + roi.height > maxSize.height) roi.height = maxSize.height - roi.y;

	return roi;
}

cv::Rect Tracker::computeBoundingBox(const std::vector<cv::Point3i>& centersIDs, int offset, int level, const cv::Size& maxSize) {
	int minX = INT_MAX, minY = INT_MAX;
	int maxX = -1, maxY = -1;

	for (auto p : centersIDs) {
			int x = p.x / pow(2, level);
		int y = p.y / pow(2, level);

		if (x < minX) minX = x;
		if (y < minY) minY = y;
		if (x > maxX) maxX = x;
		if (y > maxY) maxY = y;
	}

	minX -= offset;
	minY -= offset;
	maxX += offset;
	maxY += offset;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX > maxSize.width) maxX = maxSize.width;
	if (maxY > maxSize.height) maxY = maxSize.height;

	return {minX, minY, maxX - minX, maxY - minY};
}

void Tracker::ConvertMask(const cv::Mat& src_mask, uchar oid, cv::Mat& mask) {
	mask = cv::Mat(src_mask.size(), CV_8UC1, cv::Scalar(0));
	uchar depth = src_mask.type() & CV_MAT_DEPTH_MASK;

	if (CV_8U == depth && oid > 0) {
		for (int r = 0; r < src_mask.rows; ++r)
		for (int c = 0; c < src_mask.cols; ++c) {
			if (oid == src_mask.at<uchar>(r,c))
				mask.at<uchar>(r,c) = 255;
		}
	} else 
	if (CV_32F == depth) {
		for (int r = 0; r < src_mask.rows; ++r)
		for (int c = 0; c < src_mask.cols; ++c) {
			if (src_mask.at<float>(r,c))
				mask.at<uchar>(r,c) = 255;
		}
	}	else {
		LOG(ERROR) << "WRONG IMAGE TYPE";
	}
}

void Tracker::ConvertMask(const cv::Mat& src_mask, uchar oid, cv::Rect& roi, cv::Mat& mask) {
	mask = cv::Mat(src_mask.size(), CV_8UC1, cv::Scalar(0));
	uchar depth = src_mask.type() & CV_MAT_DEPTH_MASK;

	cv::Mat roi_src_mask = src_mask(roi);
	cv::Mat roi_mask = mask(roi);

	if (CV_8U == depth && oid > 0) {
		for (int r = 0; r < roi_src_mask.rows; ++r)
		for (int c = 0; c < roi_src_mask.cols; ++c) {
			if (oid == roi_src_mask.at<uchar>(r,c))
				roi_mask.at<uchar>(r,c) = 255;
		}
	} else if (CV_32F == depth) {
		for (int r = 0; r < roi_src_mask.rows; ++r)
		for (int c = 0; c < roi_src_mask.cols; ++c) {
			if (roi_src_mask.at<float>(r,c))
				roi_mask.at<uchar>(r,c) = 255;
		}
	}	else {
		LOG(ERROR) << "WRONG IMAGE TYPE";
	}
}

void Tracker::ShowMask(const cv::Mat& masks, cv::Mat& buf) {
	uchar depth = masks.type() & CV_MAT_DEPTH_MASK;

	if (CV_8U == depth) {
		for (int r = 0; r < masks.rows; ++r)
		for (int c = 0; c < masks.cols; ++c) {
			if (1 == masks.at<uchar>(r, c)) {
				buf.at<cv::Vec3b>(r, c)[0] = 255;
				buf.at<cv::Vec3b>(r, c)[1] = 255;
				buf.at<cv::Vec3b>(r, c)[2] = 255;
			}	else if (2 == masks.at<uchar>(r, c)) {
				buf.at<cv::Vec3b>(r, c)[0] = 128;
				buf.at<cv::Vec3b>(r, c)[1] = 128;
				buf.at<cv::Vec3b>(r, c)[2] = 128;
			}

		}
	} else {
		LOG(ERROR) << "WRONG IMAGE TYPE";
	}
}

TrackerBase::TrackerBase(const cv::Matx33f& K, std::vector<std::shared_ptr<Object3D>>& objects) 
: Tracker(K, objects) 
{
	hists = new RBOTHist(objects);
	//hists = new TestHist(objects);
	//hists = new GlobalHist(objects);
	//hists = new WTCLCHist(objects);
}

void TrackerBase::DetectEdge(const cv::Mat& img, cv::Mat& img_edge) {
	static const double CANNY_LOW_THRESH = 10;
	static const double CANNY_HIGH_THRESH = 20;

	cv::Mat img_gray;
	cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
	cv::Canny(img_gray, img_edge, CANNY_LOW_THRESH, CANNY_HIGH_THRESH);
}

void TrackerBase::PreProcess(cv::Mat frame) {
	UpdateHist(frame);
}

void TrackerBase::PostProcess(cv::Mat frame) {
	UpdateHist(frame);
}

void TrackerBase::UpdateHist(cv::Mat frame) {
	float afg = 0.1f, abg = 0.2f;
	if (initialized) {
		view->setLevel(0);
		view->RenderSilhouette(std::vector<std::shared_ptr<Model>>(objects.begin(), objects.end()), GL_FILL);
		cv::Mat masks_map = view->DownloadFrame(View::MASK);
		cv::Mat depth_map = view->DownloadFrame(View::DEPTH);

		for (int oid = 0; oid < objects.size(); oid++) {
			hists->Update(frame, masks_map, depth_map, oid, afg, abg);
		}
	}
}

SLTracker::SLTracker(const cv::Matx33f& K, std::vector<std::shared_ptr<Object3D>>& objects)
	: TrackerBase(K, objects)
{
	tk::GlobalParam* gp = tk::GlobalParam::Instance();

	search_line = std::make_shared<SearchLine>();
}

#if 1
void SLTracker::GetBundleProb(const cv::Mat& frame, int oid) {
	std::vector<std::vector<cv::Point> >& search_points = search_line->search_points;
	std::vector<std::vector<cv::Point2f> >& bundle_prob = search_line->bundle_prob;
	bundle_prob.clear();

	int level = view->getLevel();
	int upscale = pow(2, level);

	std::shared_ptr<TCLCHistograms> tclcHistograms = objects[oid]->getTCLCHistograms();
	std::vector<cv::Point3i> centersIDs = tclcHistograms->getCentersAndIDs();
	int numHistograms = (int)centersIDs.size();
	int numBins = tclcHistograms->getNumBins();
	int binShift = 8 - log(numBins) / log(2);
	int radius = tclcHistograms->getRadius();
	int radius2 = radius * radius;
	uchar* initializedData = tclcHistograms->getInitialized().data;
	uchar* frameData = frame.data;
	cv::Mat localFG = tclcHistograms->getLocalForegroundHistograms();
	cv::Mat localBG = tclcHistograms->getLocalBackgroundHistograms();
	float* histogramsFGData = (float*)localFG.ptr<float>();
	float* histogramsBGData = (float*)localBG.ptr<float>();

	cv::Mat sumsFB = tclcHistograms->sumsFB;

	for (int r = 0; r < search_points.size(); r++) {
		std::vector<cv::Point2f> slp;
		for (int c = 0; c < search_points[r].size() - 1; c++) {
			int i = search_points[r][c].x;
			int j = search_points[r][c].y;
			int pidx = j * frame.cols + i;

			int ru = (frameData[3 * pidx] >> binShift);
			int gu = (frameData[3 * pidx + 1] >> binShift);
			int bu = (frameData[3 * pidx + 2] >> binShift);

			int binIdx = (ru * numBins + gu) * numBins + bu;

			float ppf = .0f;
			float ppb = .0f;

			int cnt = 0;

			for (int h = 0; h < numHistograms; h++) {
				cv::Point3i centerID = centersIDs[h];
				if (initializedData[centerID.z]) {
					int dx = centerID.x - upscale * (i + 0.5f);
					int dy = centerID.y - upscale * (j + 0.5f);
					int distance = dx * dx + dy * dy;

					if (distance <= radius2) {
						float pf = localFG.at<float>(centerID.z, binIdx);
						float pb = localBG.at<float>(centerID.z, binIdx);

						int* sumFB = (int*)sumsFB.ptr<int>() + centerID.z * 2;
						int etaf = sumFB[0];
						int etab = sumFB[1];

						pf += 0.0000001f;
						pb += 0.0000001f;

						//ppf += etaf*pf / (etaf*pf + etab*pb);
						//ppb += etab*pb / (etaf*pf + etab*pb);

						ppf += pf / (pf + pb);
						ppb += pb / (pf + pb);

						cnt++;
					}
				}
			}

			if (cnt) {
				ppf /= cnt;
				ppb /= cnt;
			}

			slp.push_back(cv::Point2f(ppf, ppb));
		} // for cols

		bundle_prob.push_back(slp);
	} // for rows
}
#else
void RBOTHist::GetBundleProb(SearchLine* search_line, const cv::Mat& frame, int oid) {
	std::vector<std::vector<cv::Point> >& search_points = search_line->search_points;
	std::vector<std::vector<cv::Point2f> >& bundle_prob = search_line->bundle_prob;
	bundle_prob.clear();

	int level = view->getLevel();
	int upscale = pow(2, level);

	std::shared_ptr<TCLCHistograms> tclcHistograms = objs[oid]->getTCLCHistograms();
	std::vector<cv::Point3i> centersIDs = tclcHistograms->getCentersAndIDs();
	int numHistograms = (int)centersIDs.size();
	int numBins = tclcHistograms->getNumBins();
	int binShift = 8 - log(numBins) / log(2);
	int radius = tclcHistograms->getRadius();
	int radius2 = radius * radius;
	uchar* initializedData = tclcHistograms->getInitialized().data;
	uchar* frameData = frame.data;
	cv::Mat localFG = tclcHistograms->getLocalForegroundHistograms();
	cv::Mat localBG = tclcHistograms->getLocalBackgroundHistograms();
	float* histogramsFGData = (float*)localFG.ptr<float>();
	float* histogramsBGData = (float*)localBG.ptr<float>();

	cv::Mat sumsFB = tclcHistograms->sumsFB;

	for (int r = 0; r < search_points.size(); r++) {
		std::vector<cv::Point2f> slp;
		for (int c = 0; c < search_points[r].size() - 1; c++) {
			int i = search_points[r][c].x;
			int j = search_points[r][c].y;
			int pidx = j * frame.cols + i;

			int ru = (frameData[3 * pidx] >> binShift);
			int gu = (frameData[3 * pidx + 1] >> binShift);
			int bu = (frameData[3 * pidx + 2] >> binShift);

			int binIdx = (ru * numBins + gu) * numBins + bu;

			float ppf = .0f;
			float ppb = .0f;

			float cnt = 0;

			for (int h = 0; h < numHistograms; h++) {
				cv::Point3i centerID = centersIDs[h];
				if (initializedData[centerID.z]) {
					int dx = centerID.x - upscale * (i + 0.5f);
					int dy = centerID.y - upscale * (j + 0.5f);
					int distance = dx * dx + dy * dy;

					if (distance <= radius2) {
						float pf = localFG.at<float>(centerID.z, binIdx);
						float pb = localBG.at<float>(centerID.z, binIdx);

						int* sumFB = (int*)sumsFB.ptr<int>() + centerID.z * 2;
						int etaf = sumFB[0];
						int etab = sumFB[1];

						pf += 0.0000001f;
						pb += 0.0000001f;

						//ppf += etaf*pf / (etaf*pf + etab*pb);
						//ppb += etab*pb / (etaf*pf + etab*pb);

						ppf += pf / (pf + pb) * tclcHistograms->wes[h];
						ppb += pb / (pf + pb) * tclcHistograms->wes[h];

						cnt += tclcHistograms->wes[h];
					}
				}
			}

			if (cnt) {
				ppf /= cnt;
				ppb /= cnt;
			}

			slp.push_back(cv::Point2f(ppf, ppb));
		} // for cols

		bundle_prob.push_back(slp);
	} // for rows
}
#endif

void SLTracker::FilterOccludedPoint(const cv::Mat& mask, const cv::Mat& depth) {
	const std::vector<std::vector<cv::Point> >& search_points = search_line->search_points;

	for (int r = 0; r < search_points.size(); ++r) {
		int mid = search_points[r][search_points[r].size() - 1].x;

		cv::Point ptc = search_points[r][mid];
		cv::Point ptb = search_points[r][mid-1];

		uchar oidc = mask.at<uchar>(ptc);
		uchar oidb = mask.at<uchar>(ptb);

		if (oidb != 0 && oidb != oidc && depth.at<float>(ptc) < depth.at<float>(ptb)) {
			search_line->actives[r] = 0;
		}
	}
}