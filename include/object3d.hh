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

#include "model.hh"
#include "tclc_histograms.hh"

namespace ttool::tslet
{
  // forward declaration to avoid circular dependency
  class TemplateView;

  /**
   *  A representation of a 3D object that provides all nessecary information
   *  for region-based pose estimation using tclc-histograms. It extends the
   *  basic model class by including a set of tclc-histograms and a list of
   *  all corresponding templates used for pose detection.
   */
  class Object3D : public Model
  {
  public:
      /**
       *  Constructor creating a 3D object class from a specified initial 6DOF pose, a
       *  scaling factor, a tracking quality threshhold and a set of distances to the
       *  camera for template generation used within pose detection. Here, also the set
       *  of n tclc-histograms is initialized, with n being the total number of 3D model
       *  vertices.
       *
       *  @param objFilename  The relative path to an OBJ/PLY file describing the model.
       *  @param tx  The models initial translation in X-direction relative to the camera.
       *  @param ty  The models initial translation in Y-direction relative to the camera.
       *  @param tz  The models initial translation in Z-direction relative to the camera.
       *  @param alpha  The models initial Euler angle rotation about X-axis of the camera.
       *  @param beta  The models initial Euler angle rotation about Y-axis of the camera.
       *  @param gamma  The models initial Euler angle rotation about Z-axis of the camera.
       *  @param scale  A scaling factor applied to the model in order change its size independent of the original data.
       *  @param qualityThreshold  The individual quality tracking quality threshold used to decide whether tracking and detection have been successful (should be within [0.5,0.6]).
       *  @param templateDistances  A vector of absolute Z-distance values to be used for template generation (typically 3 values: a close, an intermediate and a far distance)
       */
      Object3D(const std::string objFilename, float tx, float ty, float tz, float alpha, float beta, float gamma, float scale, float qualityThreshold, std::vector<float> &templateDistances);
      Object3D(const std::string objFilename, const cv::Matx44f& Ti, float scale, float qualityThreshold, std::vector<float> &templateDistances);
      ~Object3D();

      void Init(float qualityThreshold, std::vector<float> &templateDistances);
      
      /**
       *  Returns the set of tclc-histograms associated with this object.
       *
       *  @return  The set of tclc-histograms associated with this object.
       */
      std::shared_ptr<ttool::tslet::TCLCHistograms> getTCLCHistograms();
      void SetTCLCHistograms(std::shared_ptr<ttool::tslet::TCLCHistograms> tclcHistograms);

      /**
       *  Generates all base and neighboring templates required for
       *  the pose detection algorithm after a tracking loss.
       *  Must be called after the rendering buffers of the
       *  corresponding 3D model have been initialized and while
       *  the offscreen rendering OpenGL context is active.
       */
      void generateTemplates();
      
      /**
       *  Returns the set of all pre-generated base and neighboring template views
       *  of this object used during pose detection.
       *
       *  @return  The set of all template views for this object.
       */
      std::vector<std::shared_ptr<TemplateView>> getTemplateViews();
      
      /**
       *  Returns the number of Z-distances used during template view generation
       *  for this object.
       *
       *  @return  The number of Z-distances of the template views for this object.
       */
      int getNumDistances();
      
      /**
       *  Clears all tclc-histograms and resets the pose of the object to the initial
       *  configuration.
       */
      void reset();
      
      int fcount;
  private:
      bool trackingLost;
      
      float qualityThreshold;
      
      int numDistances;
      
      std::vector<float> templateDistances;
      
      std::vector<cv::Vec3f> baseIcosahedron;
      std::vector<cv::Vec3f> subdivIcosahedron;
      
      std::shared_ptr<ttool::tslet::TCLCHistograms> tclcHistograms;
      
      std::vector<std::shared_ptr<TemplateView>> baseTemplates;
      std::vector<std::shared_ptr<TemplateView>> neighboringTemplates;
      
  };
}