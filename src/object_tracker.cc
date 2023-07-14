#include "object_tracker.hh"

using namespace tslet;

void ObjectTracker::Consume(int modelID, std::shared_ptr<Object3D> object, cv::Matx33f K)
{
    // We expect the frame to already be undistorted
    cv::Matx14f distCoeffs = cv::Matx14f(0.0, 0.0, 0.0, 0.0);
    if (HasPreviousTracker())
    {
        m_TrackerPtr->ToggleTracking(0, false);
        m_TrackerPtr->Reset(); // Go through the objects in the tracker and reset its histogram
        m_TrackerPtr.reset();
    }
    std::vector<std::shared_ptr<Object3D>>objects = {object};
    m_TrackerPtr = std::shared_ptr<Tracker>(Tracker::GetTracker(1, K, distCoeffs, objects));
    m_CurrentModelID = modelID;
    m_TrackerPtr->ToggleTracking(0, true);
    if (!HasPose(modelID))
    {
        m_ModelID2pose[modelID] = object->getPose();
    }
}

void ObjectTracker::UpdateHistogram(int modelID, cv::Mat frame)
{
    if (!HasTracker(modelID))
    {
        std::cout << "No tracker for model " << modelID << std::endl;
        return;
    }

    m_TrackerPtr->PostProcess(frame);
}

void ObjectTracker::SetPose(int modelID, cv::Matx44f pose)
{
    if (!HasTracker(modelID))
    {
        std::cout << "No tracker for model " << modelID << std::endl;
        return;
    }

    m_ModelID2pose[modelID] = pose;
}

void ObjectTracker::CallEstimatePose(int modelID, cv::Mat frame)
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

    cv::Matx44f init_pose = m_ModelID2pose[modelID];
    // cv::imshow("What Tracker Sees", frame);
    m_TrackerPtr->EstimatePoses(init_pose, frame);
}