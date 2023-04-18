#include "TSegment.h"
#include "camera.hh"
#include "global_param.hh"
#include "model.hh"
#include "pose_io.hh"
#include "visualizer.hh"
#include "input.hh"
#include "tracker.hh"
#include "d_model_manager.hh"

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
    while (true)
    {
        // 1 Tsegment
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
        }
        while (!seg.IsReady())
        {
            int key = cv::waitKey(1);
            if ('q' == key)
            {
                break;
            }
            input.ConsumeKey(key);

            visualizer.UpdateVisualizer(fid);
            seg.ConsumeImage(cameraPtr->image());

            cameraPtr->UpdateCamera();
            ++fid;
        }
        std::pair<bool, cv::Mat> mask_pair = seg.GetMask();
        cv::Mat mask = mask_pair.second;
        // cv::imshow("Segmentation Mask", mask);
        // // 2a TML
        // while (bool)
        // {
        //     images = camera_ptr->image();

        // }
        // initialPose = TML.getInitialPose(iamges, mask_pair.second);

        // 2b UI pose input
        while (true)
        {
            visualizer.UpdateVisualizer(fid);
            int key = cv::waitKey(1);
            input.ConsumeKey(key);
            // Save the pose
            if ('p' == key)
            {
                break;
            }
        }
        int initFid = 0;
        gtPoses[0][initFid] = input.GetPose();

        // 3 TSlet
        // Start tracker with camera
        cv::cvtColor(mask, mask, cv::COLOR_GRAY2RGB);

        cv::Matx14f distCoeffs = cv::Matx14f(0.0, 0.0, 0.0, 0.0);
        std::vector<std::shared_ptr<Object3D>>objects = {modelManagerPtr->GetObject()};
        std::shared_ptr<Tracker> trackerPtr(Tracker::GetTracker(gp->tracker_mode, K, distCoeffs, objects));

        trackerPtr->ToggleTracking(0, true);
		trackerPtr->PreProcess(mask);

        while (true)
        {
            int key = cv::waitKey(1);
            if ('q' == key)
            {
                break;
            }
            input.ConsumeKey(key);

            trackerPtr->EstimatePoses(gtPoses[0][initFid], mask);
            visualizer.UpdateVisualizer(fid);
            cameraPtr->UpdateCamera();
            trackerPtr->PostProcess(mask);
            ++fid;
        }
        break;
    }

    return 0;
}