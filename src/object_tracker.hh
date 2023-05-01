#pragma once

#include <opencv2/opencv.hpp>

#include "tracker.hh"
#include "object3d.hh"

namespace tslet
{
    struct ObjectTracker
    {
        public:
        void Consume(int modelID, std::shared_ptr<Object3D> object, int trackerMode, cv::Matx33f K)
        {
            cv::Matx14f distCoeffs = cv::Matx14f(0.0, 0.0, 0.0, 0.0);
            if (!hasTracker(modelID))
            {
                std::vector<std::shared_ptr<Object3D>>objects = {object};
                std::shared_ptr<Tracker> trackerPtr(Tracker::GetTracker(trackerMode, K, distCoeffs, objects));
                modelID2tracker[modelID] = trackerPtr;
                trackerPtr->ToggleTracking(0, true);
            }
        }

        void FeedNewFrame(int modelID, cv::Mat frame)
        {
            if (!hasTracker(modelID))
            {
                std::cout << "No tracker for model " << modelID << std::endl;
                return;
            }

            std::shared_ptr<Tracker> trackerPtr = modelID2tracker[modelID];
            trackerPtr->PostProcess(frame);
        }

        void SetPose(int modelID, cv::Matx44f pose)
        {
            if (!hasTracker(modelID))
            {
                std::cout << "No tracker for model " << modelID << std::endl;
                return;
            }

            modelID2pose[modelID] = pose;
        }

        void EstimatePose(int modelID, cv::Mat frame)
        {
            if (!hasTracker(modelID) || !hasPose(modelID))
            {
                std::cout << "No tracker for model/pose " << modelID << std::endl;
                return;
            }

            std::shared_ptr<Tracker> trackerPtr = modelID2tracker[modelID];
            cv::Matx44f init_pose = modelID2pose[modelID];
            cv::imshow("What Tracker Sees", frame);
            trackerPtr->EstimatePoses(init_pose, frame);
        }

        private:
        bool hasTracker(int modelID)
        {
            return modelID2tracker.find(modelID) != modelID2tracker.end();
        }

        bool hasPose(int modelID)
        {
            return modelID2pose.find(modelID) != modelID2pose.end();
        }

        private:
        std::map<int, std::shared_ptr<Tracker>> modelID2tracker;
        std::map<int, cv::Matx44f> modelID2pose;
    };
}