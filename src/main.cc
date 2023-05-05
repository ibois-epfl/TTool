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

    cameraPtr->ReadFromFile(gp->camera_config_file);

    // cv::Matx33f K = cv::Matx33f(gp->K[0], gp->K[1], gp->K[2], gp->K[3], gp->K[4], gp->K[5], gp->K[6], gp->K[7], gp->K[8]);

    // cameraPtr->SetK(K);
    // cameraPtr->SetDistCoeff(gp->dist_coeff);
    // cameraPtr->SetPreprocessSize(gp->preprocess_width, gp->preprocess_height);

	gp->image_width = cameraPtr->width;
	gp->image_height = cameraPtr->height;

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
        objectTracker.Consume(modelManagerPtr->GetObject()->getModelID(), modelManagerPtr->GetObject(), gp->tracker_mode, cameraPtr->GetK());
        modelManagerPtr->IncreaseObjectID();
    }

    // Cache segmentations for each object
    std::map<int, cv::Mat> modelID2mask;

    while (true)
    {
        // 1 Tsegment
        int oid = modelManagerPtr->GetObject()->getModelID();
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
        objectTracker.FeedNewFrame(oid, cameraPtr->image());
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

            objectTracker.EstimatePose(oid, cameraPtr->image());
            visualizer.UpdateVisualizer(fid);
            cameraPtr->UpdateCamera();
            objectTracker.FeedNewFrame(oid, cameraPtr->image());
            ++fid;

            input.ConsumeKey(key);
        }
        std::cout << "Restarting Object " << oid << " changed to >> ";
        oid = modelManagerPtr->GetObject()->getModelID();
        std::cout << oid << std::endl;
    }

    return 0;
}