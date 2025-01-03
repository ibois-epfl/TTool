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

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "signed_distance_transform2d.hh"
#include "model.hh"

namespace ttool::tslet
{
  /**
   *  @brief This class implements an statistical image segmentation model based on temporary
   *  consistent, local color histograms (tclc-histograms). Here, each histogram corresponds
   *  to a 3D vertex of a given 3D model.
   */
  class TCLCHistograms
  {
  public:
      /**
       *  @brief Constructor that allocates both normalized and not normalized foreground
       *  and background histograms for each vertex of the given 3D model.
       *
       *  @param  model The 3D model for which the histograms are being created.
       *  @param  numBins The number of bins per color channel.
       *  @param  radius The radius of the local image region in pixels used for updating the histograms.
       *  @param  offset The minimum distance between two projected histogram centers in pixels during an update.
       */
      TCLCHistograms(std::shared_ptr<ttool::tslet::Model> model, int numBins, int radius, float offset);
      
      virtual ~TCLCHistograms();
      
      /**
       *  @brief Updates the histograms from a given camera frame by projecting all histogram
       *  centers into the image and selecting those close or on the object's contour.
       *
       *  @param  frame The color frame to be used for updating the histograms.
       *  @param  mask The corresponding binary shilhouette mask of the object.
       *  @param  depth The per pixel depth map of the object used to filter histograms on the back of the object,
       *  @param  K The camera's instrinsic matrix.
       *  @param  zNear The near plane used to render the depth map.
       *  @param  zFar The far plane used to render the depth map.
       */
      virtual void update(const cv::Mat &frame, const cv::Mat &mask, const cv::Mat &depth, cv::Matx33f &K, float zNear, float zFar, float afg, float abg);
      
      /**
       *  @brief Computes updated center locations and IDs of all histograms that project onto or close
       *  to the contour based on the current object pose at a specified image pyramid level.
       *
       *  @param  mask The binary shilhouette mask of the object.
       *  @param  depth The per pixel depth map of the object used to filter histograms on the back of the object,
       *  @param  K The camera's instrinsic matrix.
       *  @param  zNear The near plane used to render the depth map.
       *  @param  zFar The far plane used to render the depth map.
       *  @param  level The image pyramid level to be used for the update.
       */
      void updateCentersAndIds(const cv::Mat &mask, const cv::Mat &depth, const cv::Matx33f &K, float zNear, float zFar, int level);
      
      /**
       *  @brief Returns all normalized forground histograms in their current state.
       *
       *  @return The normalized foreground histograms.
       */
      cv::Mat getLocalForegroundHistograms();
      
      /**
       *  @brief Returns all normalized background histograms in their current state.
       *
       *  @return The normalized background histograms.
       */
      cv::Mat getLocalBackgroundHistograms();
      
      /**
       *  @brief Returns the locations and IDs of all histogram centers that where used for the last
       *  update() or updateCentersAndIds() call.
       *
       *  @return The list of all current center locations on or close to the contour and their corresponding IDs [(x_0, y_0, id_0), (x_1, y_1, id_1), ...].
       */
      std::vector<cv::Point3i> getCentersAndIDs();
      
      /**
       *  @brief Returns a 1D binary mask of all histograms where a '1' means that the histograms
       *  corresponding to the index has been intialized before.
       *
       *  @return A 1D binary mask telling wheter each histogram has been initialized or not.
       */
      cv::Mat getInitialized();
      
      /**
       *  @brief Returns the number of histogram bin per image channel as specified in the constructor.
       *
       *  @return The number of histogram bins per channel.
       */
      int getNumBins();
      
      /**
       *  @brief Returns the number of histograms, i.e. verticies of the corresponding 3D model.
       *
       *  @return The number of histograms.
       */
      int getNumHistograms();
      
      /**
       *  @brief Returns the radius of the local image region in pixels used for updating the
       *  histograms as specified in the constructor.
       *
       *  @return The minumum distance between two projected histogram centers in pixels.
       */
      int getRadius();
      
      /**
       *  @brief Returns the minumum distance between two projected histogram centers during an update
       *  as specified in the constructor.
       *
       *  @return The minumum distance between two projected histogram centers in pixels.
       */
      float getOffset();
      
      /**
       *  @brief Clears all histograms by resetting them to zero and setting their status to
       *  uninitialized
       */
      void clear();
      
      cv::Mat sumsFB;

      void TestLine(uchar* frameRow, uchar* maskRow, int xl, int xr, float* localHistogramFG, float* localHistogramBG, float& sum_err, float& sum_all);
      float ComputeWeight(const cv::Mat& frame, const cv::Mat& mask, cv::Point3i& center, int radius);
      std::vector<float> wes;

  protected:
      int numBins;
      
      int _numHistograms;
      
      int radius;
      
      float _offset;
      
      cv::Mat notNormalizedFG;
      cv::Mat notNormalizedBG;
      
      cv::Mat normalizedFG;
      cv::Mat normalizedBG;
      
      

      cv::Mat initialized;
      
      std::shared_ptr<ttool::tslet::Model> _model;
      
      std::vector<cv::Point3i> _centersIDs;


      std::vector<cv::Point3i> computeLocalHistogramCenters(const cv::Mat &mask);
      
      std::vector<cv::Point3i> parallelComputeLocalHistogramCenters(const cv::Mat &mask, const cv::Mat &depth, const cv::Matx33f &K, float zNear, float zFar, int level);
      
      void filterHistogramCenters(int numHistograms, float offset);
  };

  /**
   * @brief The WTCLCHistograms class, I think, it is not used anymore...
   * 
   */
  class WTCLCHistograms: public TCLCHistograms {
  public:
    WTCLCHistograms(std::shared_ptr<ttool::tslet::Model> model, int numBins, int radius, float offset);
    virtual ~WTCLCHistograms();

    virtual void update(const cv::Mat& frame, const cv::Mat& mask, const cv::Mat& depth, cv::Matx33f& K, float zNear, float zFar, float afg, float abg) override;

  protected:
    ttool::tslet::SignedDistanceTransform2D* SDT2D;
  };
}