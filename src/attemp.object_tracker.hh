#pragma once

#include <opencv2/opencv.hpp>

#include "tracker.hh"
#include "object3d.hh"

namespace tslet
{
    struct ObjectTracker
    {
        public:
        void Consume(int modelID, std::shared_ptr<Object3D> object, cv::Matx33f K)
        {
            std::cout << "Consumed model " << modelID << std::endl;
            cv::Matx14f distCoeffs = cv::Matx14f(0.0, 0.0, 0.0, 0.0);
            std::vector<std::shared_ptr<Object3D>>objects = {object};
            if (trackerPtr.get() != nullptr)
            {
                trackerPtr->ToggleTracking(0, false);
                trackerPtr.reset();
                delete trackerPtr.get();
            }
            std::cout << "Consumed model " << modelID << std::endl;
            // trackerPtr* = Tracker::GetTracker(1, K, distCoeffs, objects);
            trackerPtr = std::shared_ptr<Tracker>(Tracker::GetTracker(1, K, distCoeffs, objects));
            std::cout << "Got tracker " << modelID << std::endl;
            trackerPtr->ToggleTracking(0, true);
            std::cout << "ToggleTracking " << modelID << std::endl;
            trackerModelID = modelID;
            std::cout << "Consumed model " << modelID << std::endl;
        }

        void FeedNewFrame(int modelID, cv::Mat frame)
        {
            if (modelID != trackerModelID)
            {
                std::cout << "No tracker for model " << modelID << std::endl;
                return;
            }
            if (trackerPtr.get() == nullptr)
            {
                std::cout << "Is nullptr No tracker for model " << modelID << std::endl;
                return;
            }
            std::cout << "PostProcess " << trackerPtr << " : " << trackerPtr.get() << std::endl;
            trackerPtr->PostProcess(frame);
            std::cout << "PostProcess done " << modelID << std::endl;
        }

        void SetPose(int modelID, cv::Matx44f pose)
        {
            if (modelID != trackerModelID)
            {
                std::cout << "No tracker for model " << modelID << std::endl;
                return;
            }

            modelID2pose[modelID] = pose;
        }

        void EstimatePose(int modelID, cv::Mat frame)
        {
            if (trackerPtr.get() == nullptr)
            {
                std::cout << "No tracker for model " << modelID << std::endl;
                return;
            }
            if (trackerModelID != modelID || !hasPose(modelID))
            {
                std::cout << "No tracker for modelID/pose" << modelID << std::endl;
                return;
            }

            cv::Matx44f init_pose = modelID2pose[modelID];
            // cv::imshow("What Tracker Sees", frame);
            trackerPtr->EstimatePoses(init_pose, frame);
        }

        private:
        // bool hasTracker(int modelID)
        // {
        //     return modelID2tracker.find(modelID) != modelID2tracker.end();
        // }

        bool hasPose(int modelID)
        {
            return modelID2pose.find(modelID) != modelID2pose.end();
        }

        private:
        std::shared_ptr<Tracker> trackerPtr;
        int trackerModelID;
        std::map<int, cv::Matx44f> modelID2pose;
    };
}