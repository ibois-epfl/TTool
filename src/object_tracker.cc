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

#include "object_tracker.hh"

void ttool::tslet::ObjectTracker::Consume(int modelID, std::shared_ptr<ttool::tslet::Object3D> object, cv::Matx33f K)
{
    // We expect the frame to already be undistorted
    cv::Matx14f distCoeffs = cv::Matx14f(0.0, 0.0, 0.0, 0.0);
    m_TrackerPtr = std::shared_ptr<Tracker>(Tracker::GetTracker(1, K, distCoeffs, object));
    m_CurrentModelID = modelID;
    m_TrackerPtr->ToggleTracking(object);
    if (!HasPose(modelID))
    {
        m_ModelID2pose[modelID] = object->getPose();
    }
}

void ttool::tslet::ObjectTracker::UpdateHistogram(std::shared_ptr<ttool::tslet::Object3D> object, int modelID, cv::Mat frame)
{
    if (!HasTracker(modelID))
    {
        std::cout << "No tracker for model " << modelID << std::endl;
        return;
    }

    m_TrackerPtr->UpdateHistogram(frame, object);
}

void ttool::tslet::ObjectTracker::SetPose(int modelID, cv::Matx44f pose)
{
    if (!HasTracker(modelID))
    {
        std::cout << "No tracker for model " << modelID << std::endl;
        return;
    }

    m_ModelID2pose[modelID] = pose;
}

void ttool::tslet::ObjectTracker::CallEstimatePose(std::shared_ptr<ttool::tslet::Object3D> object, int modelID, cv::Mat frame)
{
    if (!HasTracker(modelID))
    {
        std::cout << "No tracker for model " << modelID << std::endl;
        return;
    }
    else if (!HasPose(modelID))
    {
        std::cout << "No pose for model " << modelID << std::endl;
        return;
    }

    cv::Matx44f initialPose = m_ModelID2pose[modelID];
    m_TrackerPtr->EstimatePoses(object, initialPose, frame);
}