#include "TSegment.h"
#include "camera.hh"
#include "global_param.hh"
#include "model.hh"
#include "pose_io.hh"
#include "visualizer.hh"
#include "input.hh"
#include "tracker.hh"
#include "d_model_manager.hh"
#include "object_tracker.hh"

#include <algorithm>
#include <iostream>
#include <QApplication>
#include <QThread>


int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    std::string config_file = argv[1];

    tk::GlobalParam* gp = tk::GlobalParam::Instance();
	gp->ParseConfig(config_file);

	std::shared_ptr<Camera> cameraPtr;
    if (gp->frames == "")
    {
        std::cout << "Using camera: " << gp->camera_id << std::endl;
        cameraPtr.reset(Camera::BuildCamera(gp->camera_id));
    }
    else
    {
        std::cout << "Using frames: " << gp->frames << std::endl;
        cameraPtr.reset(Camera::BuildCamera(gp->frames));
    }

	gp->image_width = cameraPtr->width;
	gp->image_height = cameraPtr->height;
    cv::Matx33f K = cv::Matx33f(gp->fx, 0, gp->cx, 0, gp->fy, gp->cy, 0, 0, 1);

 	// Set the pose reader
	std::unique_ptr<PoseReader> poseReader(new PoseReaderRBOT);
	std::vector<std::vector<cv::Matx44f> > gtPoses;
	poseReader->Read(gp->gt_pose_file, gtPoses);

    std::shared_ptr<ttool::DModelManager> modelManagerPtr = std::make_shared<ttool::DModelManager>(gp->model_file, gtPoses);
    // Initialize the visualizer
    ttool::Visualizer visualizer = ttool::Visualizer(gp, cameraPtr, modelManagerPtr);
    cameraPtr->UpdateCamera();

    ttool::Input input(modelManagerPtr);
    input.SetPoseOutput(gp->input_pose_file);

    // Initialize the tracker
    tslet::ObjectTracker objectTracker;
    for (int i = 0; i < modelManagerPtr->GetNumObjects(); ++i)
    {
        objectTracker.Consume(modelManagerPtr->GetObject()->getModelID(), modelManagerPtr->GetObject(), gp->tracker_mode, K);
        modelManagerPtr->IncreaseObjectID();
    }

    // Cache segmentations for each object
    std::map<int, cv::Mat> modelID2mask;

    while (true)
    {
        // 1 Tsegment
        int oid = modelManagerPtr->GetObject()->getModelID();
        std::string dubugPath = "/home/tpp/IBOIS/TTool/debug";
        auto seg = tsegment::Segmentation("");
        int fid = 0;
        int key = cv::waitKey(1);
        while ('p' != key && gp->frames == "")
        {
            if ('q' == key)
            {
                break;
            }
            input.ConsumeKey(key);
            visualizer.UpdateVisualizer(fid);
            cameraPtr->UpdateCamera();
            key = cv::waitKey(1);
            oid = modelManagerPtr->GetObject()->getModelID();
        }

        // std::cout << "Calculating segmentation mask for object " << oid << std::endl;
        // while (modelID2mask.find(oid) == modelID2mask.end() && !seg.IsReady())
        // {
        //     int key = cv::waitKey(1);
        //     input.ConsumeKey(key);

        //     visualizer.UpdateVisualizer(fid);
        //     seg.ConsumeImage(cameraPtr->image());

        //     cameraPtr->UpdateCamera();
        //     ++fid;
        // }

        // if (modelID2mask.find(oid) == modelID2mask.end())
        // {
        //     std::pair<bool, cv::Mat> mask_pair = seg.GetMask();
        //     cv::Mat mask = mask_pair.second;
        //     cv::cvtColor(mask, mask, cv::COLOR_GRAY2RGB);

        //     modelID2mask.insert(std::make_pair(oid, mask));
        // }
        cv::Mat mask = cameraPtr->image();
        // cv::Mat mask = modelID2mask[oid];

        // // 2a TML
        // while (bool)
        // {
        //     images = camera_ptr->image();

        // }
        // initialPose = TML.getInitialPose(iamges, mask_pair.second);

        // 2b UI pose input
        std::cout << "Please input the pose of the object" << std::endl;
        while (oid == modelManagerPtr->GetObject()->getModelID())
        {
            visualizer.UpdateVisualizer(fid);
            int key = cv::waitKey(1);
            // Save the pose
            if ('p' == key)
            {
                break;
            }
            objectTracker.SetPose(modelManagerPtr->GetObject()->getModelID(), input.GetPose());

            input.ConsumeKey(key);
        }
        // 3 TSlet
        // auto mask = cameraPtr->image();
        bool useMask = false;
        if (useMask)
            objectTracker.FeedNewFrame(oid, mask);
        else
            objectTracker.FeedNewFrame(oid, cameraPtr->image());
        cv::imshow("Segmentation Mask 137", mask);
        while (oid == modelManagerPtr->GetObject()->getModelID())
        {
            int key = cv::waitKey(1);
            if ('q' == key)
            {
                break;
            }
            // Break before the new object's gtID is used to estimate the pose of the previous object (snapshotted by the tracker)
            if (oid != modelManagerPtr->GetObject()->getModelID())
            {
                break;
            }

            if (useMask)
                objectTracker.EstimatePose(oid, mask);
            else
                objectTracker.EstimatePose(oid, cameraPtr->image());
            visualizer.UpdateVisualizer(fid);
            cameraPtr->UpdateCamera();
            if (useMask)
                objectTracker.FeedNewFrame(oid, mask);
            else
                objectTracker.FeedNewFrame(oid, cameraPtr->image());
            // mask = cameraPtr->image();
            ++fid;

            input.ConsumeKey(key);
        }
        std::cout << "Restarting Object " << oid << " changed to >> ";
        oid = modelManagerPtr->GetObject()->getModelID();
        std::cout << oid << std::endl;
    }

    return 0;
}