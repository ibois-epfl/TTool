#include "TSegment.h"
#include "camera.hh"
#include "global_param.hh"
#include "model.hh"
#include "pose_io.hh"
#include "visualizer.hh"

#include <iostream>
#include <QApplication>
#include <QThread>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    std::string config_file = "/home/tpp/IBOIS/TTool/model_drill_scaled/config.yml";

    tk::GlobalParam* gp = tk::GlobalParam::Instance();
	gp->ParseConfig(config_file);

	std::shared_ptr<Camera> cameraPtr(Camera::BuildCamera(gp->frames));  // --> "" to open live stream / original: Camera::BuildCamera(gp->frames)
	gp->image_width = cameraPtr->width;
	gp->image_height = cameraPtr->height;
    cv::Matx33f K = cv::Matx33f(gp->fx, 0, gp->cx, 0, gp->fy, gp->cy, 0, 0, 1);
 
 	// Set the pose reader
	std::unique_ptr<PoseReader> poseReader(new PoseReaderRBOT);
	std::vector<std::vector<cv::Matx44f> > gtPoses;
	poseReader->Read(gp->gt_pose_file, gtPoses);

	// Initialize object(s) to with model file, starting pose, ...
	int initFid = gp->target_frame - 1 >= 0 ? gp->target_frame - 1 : gp->target_frame;
	std::vector<std::shared_ptr<Model>> objects;
	for (int i = 0; i < gp->model_file.size(); ++i)
    {
        std::shared_ptr<Model> modelPtr = std::make_shared<Model>(gp->model_file[i],
                                                                  gtPoses[i][initFid],
                                                                  1.0);
        objects.push_back(modelPtr);
	}

    ttool::Visualizer visualizer = ttool::Visualizer(gp, cameraPtr, std::vector<std::shared_ptr<Model>>(objects.begin(), objects.end()));

    while (true)
    {
        // 1 Tsegment
        std::string dubugPath = "/home/tpp/IBOIS/TTool/debug";
        auto seg = tsegment::Segmentation("");
        int fid = 0;
        while (true)
        {
            cameraPtr->UpdateCamera();
            visualizer.UpdateVisualizer(fid);
            fid++;
            seg.ConsumeImage(cameraPtr->image());
            if (seg.IsReady())
            {
                // Create new thread and call ->
                auto mask_pair = seg.GetMask();
                cv::imshow("Mask", mask_pair.second);
                cv::waitKey(0);
                break;
            }
            if (cv::waitKey(1) == 27)
                break;

            // step: 1
        }

        // // 2a TML
        // while (bool)
        // {
        //     images = camera_ptr->image();

        // }
        // initialPose = TML.getInitialPose(iamges, mask_pair.second);

        // // 2b UI pose input
        // while (bool)
        // {
        //     pose = UI.getPose();
        // }

        // // 3 TSlet
        // while (user is happy with the tracking)
        // {
        //     pose = TSlet.track(initial, mask)
        //     visualize(pose, camera_ptr->image());
        // }
        break;
    }

    return 0;
}