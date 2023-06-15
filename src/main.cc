// #include <memory>
#include <iostream>
#include <QApplication>
#include <QThread>

#include "camera.hh"
#include "ttool.hh"
#include "util.hh"

#include "pose_writer.hh"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    std::shared_ptr<ttool::TTool> ttool = std::make_shared<ttool::TTool>(__TTOOL_CONFIG_PATH__);

    std::shared_ptr<ttool::Config> configPtr = ttool->GetConfig();

    // Initialize the camera
    std::shared_ptr<ttool::standaloneUtils::Camera> cameraPtr;
    if (configPtr->GetConfigData().Frames == "")
    {
        std::cout << "Using camera: " << configPtr->GetConfigData().CameraID << std::endl;
        cameraPtr.reset(ttool::standaloneUtils::Camera::BuildCamera(configPtr->GetConfigData().CameraID));
    }
    else
    {
        std::cout << "Using frames: " << configPtr->GetConfigData().Frames << std::endl;
        cameraPtr.reset(ttool::standaloneUtils::Camera::BuildCamera(configPtr->GetConfigData().Frames));
    }
    cameraPtr->ReadFromFile(configPtr->GetConfigData().CameraConfigFile);

    // Initialize the visualizer as standalone utility
    std::shared_ptr<ttool::standaloneUtils::Visualizer> visualizerPtr = std::make_shared<ttool::standaloneUtils::Visualizer>(cameraPtr, ttool->GetModelManager(), configPtr->GetConfigData().Zn, configPtr->GetConfigData().Zf);
    visualizerPtr->SetSaveImagePath(configPtr->GetConfigData().SaveImagePath);
    cameraPtr->UpdateCamera();

    ttool::InputVisualizer inputVisualizer(visualizerPtr);

    // PoseWriter
    PoseWriter poseWriter("trackingPose.txt", __TTOOL_CONFIG_PATH__, configPtr->GetConfigData().ModelFiles);

    // TODO:
    // the ttool needs:
    // - a) it needs index of the model -> TTool::ManipulateModel
    // - b) it needs the frame -> TTool::RunOnAFrame
    // - c) it needs the initial/modified pose -> TTool::ManipulateModel
    // after we can start tracking
    // main thread
    bool exit = false;
    while (!exit)
    {
        int fid = 0;

        // 2b UI pose input
        std::cout << "Please input the pose of the object" << std::endl;
        visualizerPtr->SetModels();
        visualizerPtr->UpdateEvent(ttool::EventType::PoseInput);
        while (!exit)
        {
            visualizerPtr->UpdateVisualizer(fid);
            cameraPtr->UpdateCamera();
            int key = cv::waitKey(1);
            // Save the pose
            if ('p' == key)
            {
                break;
            }
            if (27 == key)
            {
                exit = true;
                break;
            }
            ttool->ManipulateModel(key);
            inputVisualizer.ConsumeKey(key);
            visualizerPtr->SetModels();
        }
        // 3 Pose refiner
        visualizerPtr->UpdateEvent(ttool::EventType::Tracking);
        while (!exit)
        {
            int key = cv::waitKey(1);


            if ('q' == key)
            {
                break;
            }
            if (27 == key)
            {
                exit = true;
                break;
            }

            auto ti = cv::getTickCount();

            ttool->RunOnAFrame(cameraPtr->image());

            auto tf = cv::getTickCount();
            auto t = (tf - ti) / cv::getTickFrequency();
            int fps = 1 / t;

            visualizerPtr->UpdateVisualizer(fid, fps);
            cameraPtr->UpdateCamera();
            inputVisualizer.ConsumeKey(key);
            ++fid;

            cv::Matx44f pose = ttool->GetPose();
            poseWriter.write(pose, ttool->GetCurrentObjectID());
        }
    }

    cv::destroyAllWindows();
    ttool::makeVideoFromAllSavedImages(configPtr->GetConfigData().SaveImagePath);

}
