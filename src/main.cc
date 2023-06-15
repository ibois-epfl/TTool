// #include <memory>
#include <iostream>
#include <QApplication>
#include <QThread>

#include "camera.hh"
#include "ttool.hh"
#include "util.hh"


int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    std::shared_ptr<ttool::TTool> ttool = std::make_shared<ttool::TTool>(__TTOOL_CONFIG_PATH__);

    std::shared_ptr<ttool::Config> configPtr = ttool->GetConfig();

    // Initialize the camera
    std::shared_ptr<Camera> cameraPtr;
    if (configPtr->GetConfigData().Frames == "")
    {
        std::cout << "Using camera: " << configPtr->GetConfigData().CameraID << std::endl;
        cameraPtr.reset(Camera::BuildCamera(configPtr->GetConfigData().CameraID));
    }
    else
    {
        std::cout << "Using frames: " << configPtr->GetConfigData().Frames << std::endl;
        cameraPtr.reset(Camera::BuildCamera(configPtr->GetConfigData().Frames));
    }
    cameraPtr->ReadFromFile(configPtr->GetConfigData().CameraConfigFile);

    // Initialize the visualizer as standalone utility
    std::shared_ptr<ttool::Visualizer> visualizerPtr = std::make_shared<ttool::Visualizer>(cameraPtr, ttool->GetModelManager(), configPtr->GetConfigData().Zn, configPtr->GetConfigData().Zf);
    visualizerPtr->SetSaveImagePath(configPtr->GetConfigData().SaveImagePath);
    cameraPtr->UpdateCamera();

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
            visualizerPtr->SetModels();
        }
        // 3 TSlet
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
            ++fid;
        }
    }

    cv::destroyAllWindows();
    ttool::makeVideoFromAllSavedImages(configPtr->GetConfigData().SaveImagePath);

}
