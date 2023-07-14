#include <opencv2/highgui.hpp>
#include "view.hh"
#include "search_line.hh"
#include "histogram.hh"

Histogram::Histogram() {
	view = View::Instance();
}

Histogram::~Histogram() {}

RBOTHist::RBOTHist(const std::shared_ptr<Object3D> object) {
	m_Object = object;
	if (object->getTCLCHistograms() == nullptr)
		object->SetTCLCHistograms(std::make_shared<TCLCHistograms>(TCLCHistograms(object, 32, 40, 10.0f)));
	else
		std::cout << "RBOTHist::RBOTHist: TCLCHistograms already exists!" << std::endl;
}

void RBOTHist::Update(const cv::Mat& frame, cv::Mat& mask_map, cv::Mat& depth_map, float afg, float abg){
	float zNear = view->GetZNear();
	float zFar = view->GetZFar();
	cv::Matx33f K = view->GetCalibrationMatrix().get_minor<3, 3>(0, 0);

	m_Object->getTCLCHistograms()->update(frame, mask_map, depth_map, K, zNear, zFar, afg, abg);
}

void RBOTHist::GetPixelProb(uchar rc, uchar gc, uchar bc, int x, int y, float& ppf, float& ppb) {
	std::shared_ptr<TCLCHistograms> tclcHistograms = m_Object->getTCLCHistograms();

	std::vector<cv::Point3i> centersIDs = tclcHistograms->getCentersAndIDs();
	uchar* initializedData = tclcHistograms->getInitialized().data;

	int radius = tclcHistograms->getRadius();
	int radius2 = radius * radius;

	cv::Mat localFG = tclcHistograms->getLocalForegroundHistograms();
	cv::Mat localBG = tclcHistograms->getLocalBackgroundHistograms();
	float* histogramsFGData = (float*)localFG.ptr<float>();
	float* histogramsBGData = (float*)localBG.ptr<float>();

	int level = view->GetLevel();
	int upscale = pow(2, level);

	int numHistograms = (int)centersIDs.size();
	int numBins = tclcHistograms->getNumBins();
	int binShift = 8 - log(numBins) / log(2);

	int ru = (bc >> binShift);
	int gu = (gc >> binShift);
	int bu = (rc >> binShift);

	int binIdx = (ru * numBins + gu) * numBins + bu;

	int cnt = 0;
	ppf = .0f;
	ppb = .0f;

	for (int h = 0; h < numHistograms; h++) {
		cv::Point3i centerID = centersIDs[h];
		if (initializedData[centerID.z]) {
			int dx = centerID.x - upscale * (x + 0.5f);
			int dy = centerID.y - upscale * (y + 0.5f);
			int distance = dx * dx + dy * dy;

			if (distance <= radius2) {
				float pf = localFG.at<float>(centerID.z, binIdx);
				float pb = localBG.at<float>(centerID.z, binIdx);

				//int* sumFB = (int*)sumsFB.ptr<int>() + centerID.z * 2;
				//int etaf = sumFB[0];
				//int etab = sumFB[1];

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
}

void RBOTHist::GetRegionProb(const cv::Mat& frame, cv::Mat& prob_map) {
	prob_map = cv::Mat(frame.size(), CV_8UC1);

	int level = view->GetLevel();
	int upscale = pow(2, level);
	
	std::shared_ptr<TCLCHistograms> tclcHistograms = m_Object->getTCLCHistograms();
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

	for (int r = 0; r < frame.rows; r++)
	for (int c = 0; c < frame.cols; c++) {
		int i = c;
		int j = r;
		int pidx = j * frame.cols + i;

		int ru = frameData[3 * pidx] >> binShift;
		int gu = frameData[3 * pidx + 1] >> binShift;
		int bu = frameData[3 * pidx + 2] >> binShift;

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

					pf += 0.0000001f;
					pb += 0.0000001f;

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

		prob_map.at<uchar>(r, c) = ppf * 255;

		//prob_map.at<uchar>(r, c) = (ppf > ppb) ? 255 : 0;

	}
}