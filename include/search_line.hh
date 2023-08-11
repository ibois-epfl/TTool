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

namespace ttool::tslet
{
	/**
	 * @brief This class is used to search line used by the SLET algorithm
	 * 
	 */
	class SearchLine
	{
	public:
		SearchLine();
		virtual ~SearchLine() {}

		void FindSearchLine(const cv::Mat& mask, const cv::Mat& frame, int len, int seg, bool use_all);
		void DrawSearchLine(cv::Mat& line_mask) const;
		void DrawContours(cv::Mat& contour_mask) const;

		std::vector<std::vector<cv::Point> > contours;
		std::vector<std::vector<cv::Point> > search_points;
		std::vector<std::vector<cv::Point2f> > bundle_prob;
		std::vector<uchar> actives;
		std::vector<cv::Point2f> norms;

	protected:
		void getLine(float k, const cv::Point& center, int len, const cv::Mat& mask, std::vector<cv::Point>& search_points, cv::Point2f& norm);
		void FindContours(const cv::Mat& projection_mask, int seg, bool all_contours);
	};
}