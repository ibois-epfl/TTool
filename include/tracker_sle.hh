/**
 * This file has been modified by Andrea Settimi, Naravich Chutisilp (IBOIS, EPFL) 
 * from SLET with Copyright (C) 2020  Hong Huang and Fan Zhong and Yuqing Sun and Xueying Qin (Shandong University)
 *                     
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "tracker.hh"

namespace ttool::tslet
{
	/**
	 * @brief This class implement the SLET algorithm. This is what ttool::TTool uses.
	 * 
	 */
	class SLETracker: public SLTracker {
	public:
		SLETracker(const cv::Matx33f& K, std::shared_ptr<ttool::tslet::Object3D> objects);
		virtual void EstimatePoses(std::shared_ptr<ttool::tslet::Object3D> object, cv::Matx44f& initialPose, cv::Mat& frame) override;

	protected:
		virtual void Track(std::vector<cv::Mat>& imagePyramid, std::shared_ptr<ttool::tslet::Object3D> objects, int runs, cv::Matx44f& initialPose) override;

		void RunIteration(std::shared_ptr<ttool::tslet::Object3D> objects, const std::vector<cv::Mat>& imagePyramid, cv::Matx44f& initialPose);
		void RunIteration(std::shared_ptr<ttool::tslet::Object3D> objects, const std::vector<cv::Mat>& imagePyramid, int level, int sl_len, int sl_seg, cv::Matx44f& initialPose);
		void ComputeJac(std::shared_ptr<ttool::tslet::Object3D> object, int m_id, const cv::Mat& frame, const cv::Mat& depth_map, const cv::Mat& depth_inv_map, cv::Matx66f& wJTJM, cv::Matx61f& JTM);
		void FindMatchPointMaxProb(float diff);

	protected:
	};
}