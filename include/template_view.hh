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

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "view.hh"
#include "object3d.hh"
#include "tclc_histograms.hh"
#include "signed_distance_transform2d.hh"

namespace ttool::tslet {
    /**
     *  @brief The template view data per pixel.
     */
    struct PixelData
    {
        /// @brief The original 2D pixel location.
        int x;
        int y;
        
        /// @brief The Heaviside value.
        float hsVal;
        
        /// @brief The number of tclc-histograms the pixel lies within.
        int ids_size;
        
        /// @brief The IDs of all tclc-histograms the pixel lies within.
        int* ids;
    };

    /**
     *  @brief A class representing a single template view at multiple image scales
     *  for region-based object pose detection using tclc-histograms.
     */
    class TemplateView {
        
    public:
        /**
         *  @brief Constructor for the template view at a given object rotation and
         *  distance to the camera.
         *
         *  @param  object The 3D object for which the template view is to be created.
         *  @param  alpha The Euler angle of the object's rotation around the x-axis (in degrees).
         *  @param  beta The Euler angle of the object's rotation around the y-axis (in degrees).
         *  @param  gamma The Euler angle of the object's rotation around the z-axis (in degrees).
         *  @param  distance The object's distance to the camera to be used.
         *  @param  numLevels Number of template pyramid levels to be created with a downscale factor of 2.
         *  @param  generateNeighbors A flag telling whether neighboring templates should also be created or not.
         */
        TemplateView(std::shared_ptr<ttool::tslet::Object3D> object, float alpha, float beta, float gamma, float distance, int numLevels, bool generateNeighbors);
        
        ~TemplateView();
        
        /**
         *  @brief Returns the 6DOF object pose coresponding to the
         *  template view in form of a 4x4 float matrix
         *  T_cm = [r11 r12 r13 tx]
         *         [r21 r22 r23 ty]
         *         [r31 r32 r33 tz]
         *         [  0   0   0  1],
         *  describing the transformation from model coordinates X_m
         *  into camera coordinates X_c.
         *
         *  @return  The 6DOF object pose within the template.
         */
        cv::Matx44f getPoseMatrix();
        
        /**
         *  @brief Returns the Euler angle of the object's rotation around the
         *  x-axis coresponding to the template.
         *
         *  @return  The object's rotation around the x-axis within the template (in degrees).
         */
        float getAlpha();
        
        /**
         *  @brief Returns the Euler angle of the object's rotation around the
         *  y-axis coresponding to the template.
         *
         *  @return  The object's rotation around the y-axis within the template (in degrees).
         */
        float getBeta();
        
        /**
         *  @brief Returns the Euler angle of the object's rotation around the
         *  z-axis coresponding to the template.
         *
         *  @return  The object's rotation around the z-axis within the template (in degrees).
         */
        float getGamma();
        
        /**
         *  @brief Returns the object's distance to the camera coresponding to
         *  the template.
         *
         *  @return  The object's distance to the camera within the template.
         */
        float getDistance();
        
        /**
         *  @brief Returns the total number of pixels in the object region at a given
         *  pyramid level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The total number of pixels in the object region within the template.
         */
        int getEtaF(int level);
        
        /**
         *  @brief Returns the binary mask image of the template at a given pyramid
         *  level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The binary mask image of the template.
         */
        cv::Mat getMask(int level);
        
        /**
         *  @brief Returns the 2D signed distance transform of the binary mask of the
         *  template at a given pyramid level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The 2D signed distance transform of the binary mask of the template.
         */
        cv::Mat getSDT(int level);
        
        /**
         *  @brief Returns the smoothed Heaviside representation of the 2D signed distance
         *  transform of the template at a given pyramid level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The smoothed Heaviside representation of the 2D signed distance transform of the template.
         */
        cv::Mat getHeaviside(int level);
        
        /**
         *  @brief Returns the 2D region of interest around the object in the template at
         *  a given pyramid level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The 2D region of interest around the object in the template.
         */
        cv::Rect getROI(int level);
        
        /**
         *  @brief Returns the 2D (x, y) offset at which the template currently matches
         *  best with the corresponding matching score at a given pyramid level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The 2D offset with the matching score (x, y, score).
         */
        cv::Point3f getCurrentOffset(int level);
        
        /**
         *  @brief Sets the 2D (x, y) offset at which the template currently matches
         *  best with the corresponding matching score at a given pyramid level.
         *
         *  @param offset The 2D offset with the matching score (x, y, score).
         *  @param level The pyramid level to be used.
         */
        void setCurrentOffset(cv::Point3f &offset, int level);
        
        /**
         *  @brief Returns the 2D centers and IDs of all tclc-histograms in the
         *  template at a given pyramid level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The 2D centers and IDs of all tclc-histograms in the template [(x_0, y_0, id_0), (x_1, y_1, id_1), ...].
         */
        std::vector<cv::Point3i> getCentersAndIDs(int level);
        
        /**
         *  @brief Returns a linearized representation of the template at a given pyramid
         *  level.
         *
         *  @param level The pyramid level to be used.
         *  @return  The linearized representation of the template.
         */
        std::vector<PixelData> &getCompressedPixelData(int level);
        
        /**
         *  @brief Adds a neighboring template view to this template.
         *
         *  @param kv The neighboring template view to be added.
         */
        void addNeighborTemplate(std::shared_ptr<TemplateView> kv);
        
        /**
         *  @brief Returns the set of all neighboring templates of this template.
         *
         *  @return  The set of all neighboring templates of this template.
         */
        std::vector<std::shared_ptr<TemplateView>> getNeighborTemplates();
        
    private:
        View *view;
        
        cv::Matx44f T_cm;
        
        std::vector<int> etaFPyramid;
        std::vector<cv::Mat> maskPyramid;
        std::vector<cv::Mat> sdtPyramid;
        std::vector<cv::Mat> heavisidePyramid;
        
        std::vector<cv::Rect> roiPyramid;
        
        std::vector<std::vector<cv::Point3i> > centersIDsPyramid;
        
        std::vector<std::vector<PixelData> > pixelDataPyramid;
        
        cv::Point3f currentOffset;
        
        float _alpha;
        float _beta;
        float _gamma;
        
        float _distance;
        
        int _numLevels;
        
        std::vector<std::shared_ptr<TemplateView>> neighbors;
        
        void compressTemplateData(const std::vector<cv::Point3i> &centersIDs, const cv::Mat &heaviside, const cv::Rect &roi, int radius, int level);
        
        cv::Rect computeBoundingBox(const std::vector<cv::Point3i> &centersIDs, int offset, int level, const cv::Size &maxSize);
    };


    /**
     *  @brief This class extends the OpenCV ParallelLoopBody for efficiently parallelized
     *  computations. Within the corresponding for loop, every pixel of a given 2D
     *  signed distance transform is mapped to its correspoding value in the
     *  smoothed Heaviside function representation.
     */
    class Parallel_For_convertToHeaviside: public cv::ParallelLoopBody
    {
    private:
        cv::Mat _sdt, _heaviside;
        
        float *sdtData, *hsData;
        
        int _threads;
        
    public:
        Parallel_For_convertToHeaviside(const cv::Mat &sdt, cv::Mat &heaviside, int threads)
        {
            _sdt = sdt;
            
            heaviside.create(_sdt.rows, _sdt.cols, CV_32FC1);
            _heaviside = heaviside;
            
            sdtData = _sdt.ptr<float>();
            hsData = _heaviside.ptr<float>();
            
            _threads = threads;
        }
        
        virtual void operator()( const cv::Range &r ) const
        {
            int range = _sdt.rows/_threads;
            
            int yEnd = r.end*range;
            if(r.end == _threads)
            {
                yEnd = _sdt.rows;
            }
            
            float s = 1.2f;
            
            for(int y = r.start*range; y < yEnd; y++)
            {
                float* sdtRow = sdtData + y*_sdt.cols;
                float* hsRow = hsData + y*_heaviside.cols;
                
                for(int x = 0; x < _sdt.cols; x++)
                {
                    float dist = sdtRow[x];
                    hsRow[x] = (fabs(dist) <= 8.0f) ? 1.0f/float(CV_PI)*(-atan(dist*s)) + 0.5f : -1.0f;
                }
            }
        }
    };
}