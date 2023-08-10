/**
 * TTool
 * Copyright (C) 2023  Andrea Settimi, Naravich Chutisilp (IBOIS, EPFL)
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

#include <opencv2/opencv.hpp>

#include "tracker.hh"
#include "object3d.hh"

namespace tslet
{
    /**
     * @brief This class is a wrapper around the tracker class. It is used to track a single object
     * 
     */
    struct ObjectTracker
    {
        public:
        /**
         * @brief Set up a tracker for the given object
         * This function is called to specify the object to be tracked
         * 
         * @param modelID the ID of the object to be tracked that should correspond to the ID in the ModelManager
         * @param object  the object to be tracked
         * @param K       the camera matrix
         */
        void Consume(int modelID, std::shared_ptr<Object3D> object, cv::Matx33f K);

        /**
         * @brief Call the UpdateHistogram function of the tracker which update the TCLC histogram from the projected model on the frame
         * 
         * @param object  the object to be tracked
         * @param modelID the ID of the object to be tracked that should correspond to the ID in the ModelManager
         * @param frame   the current frame from the camera
         */
        void UpdateHistogram(std::shared_ptr<Object3D> object, int modelID, cv::Mat frame);

        /**
         * @brief Set the Pose object
         * 
         * @param modelID the ID of the object to be tracked that should correspond to the ID in the ModelManager
         * @param pose    the pose of the object to be tracked
         */
        void SetPose(int modelID, cv::Matx44f pose);

        /**
         * @brief Call the EstimatePose function of the tracker which estimate the pose of the object to be tracked
         * 
         * @param object  the object to be tracked
         * @param modelID the ID of the object to be tracked that should correspond to the ID in the ModelManager
         * @param frame   the current frame from the camera
         */
        void CallEstimatePose(std::shared_ptr<Object3D>object, int modelID, cv::Mat frame);

        /**
         * @brief Get the Tracking Status from the tracker
         * 
         * @return std::string including Reset, Freezed and Tracking with the average of search line scores
         */
        std::string GetTrackingStatus()
        {
            return m_TrackerPtr->GetTrackingStatus();
        }

        private:
        /**
         * @brief Check if the tracker is already set up for the given object
         * 
         * @param modelID the ID of the object to be tracked that should correspond to the ID in the ModelManager
         * @return true 
         * @return false 
         */
        bool HasTracker(int modelID)
        {
            return m_CurrentModelID == modelID;
        }

        /**
         * @brief Check if there is a previous tracker that should be reset
         * 
         * @return true 
         * @return false 
         */
        bool HasPreviousTracker()
        {
            return m_CurrentModelID != -1;
        }

        /**
         * @brief Check if the pose of the object to be tracked is already set
         * 
         * @param modelID 
         * @return true 
         * @return false 
         */
        bool HasPose(int modelID)
        {
            return m_ModelID2pose.find(modelID) != m_ModelID2pose.end();
        }

        private:
        std::shared_ptr<Tracker> m_TrackerPtr;
        int m_CurrentModelID = -1;
        std::map<int, cv::Matx44f> m_ModelID2pose;
    };
}