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
#include <vector>
#include "object3d.hh"
#include "tclc_histograms.hh"

class View;

namespace ttool::tslet
{
	class Histogram {
	public:
		Histogram();
		virtual ~Histogram() = 0;

		virtual void Update(std::shared_ptr<ttool::tslet::Object3D> object, const cv::Mat& frame, cv::Mat& mask_map, cv::Mat& depth_map, float afg, float abg)  = 0;
		virtual void GetPixelProb(std::shared_ptr<ttool::tslet::Object3D> object, uchar rc, uchar gc, uchar bc, int x, int y, float& ppf, float& ppb) = 0;
		virtual void GetRegionProb(std::shared_ptr<ttool::tslet::Object3D> object, const cv::Mat& frame, cv::Mat& prob_map) = 0;

	protected:
		View* view;
		
	};

	class RBOTHist : public Histogram {
	public:
		RBOTHist(const std::shared_ptr<ttool::tslet::Object3D> object);

		virtual void Update(std::shared_ptr<ttool::tslet::Object3D> object, const cv::Mat& frame, cv::Mat& mask_map, cv::Mat& depth_map, float afg, float abg) override;
		virtual void GetPixelProb(std::shared_ptr<ttool::tslet::Object3D> object, uchar rc, uchar gc, uchar bc, int x, int y, float& ppf, float& ppb) override;
		virtual void GetRegionProb(std::shared_ptr<ttool::tslet::Object3D> object, const cv::Mat& frame, cv::Mat& prob_map) override;
	};
}