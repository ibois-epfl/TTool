// #include "camera.hh"
// #include "model.hh"
// #include "pose_io.hh"
// #include "visualizer.hh"
// #include "input.hh"
// #include "tracker.hh"
// #include "d_model_manager.hh"
// #include "object_tracker.hh"
// #include "event.hh"
// #include "util.hh"

// #include <algorithm>
// #include <iostream>
// #include <QApplication>
// #include <QThread>


// int main(int argc, char **argv)
// {
//     QApplication a(argc, argv);

//     std::string config_file = __TTOOL_CONFIG_PATH__;

//     // Initialize the camera
//     // TODO: this part shpuld be refactored to read directly frames from a camera cv::VideoCapture
//     std::shared_ptr<ttool::Config> configPtr = std::make_shared<ttool::Config>(config_file);
//     std::shared_ptr<Camera> cameraPtr;
//     if (configPtr->GetConfigData().Frames == "")
//     {
//         std::cout << "Using camera: " << configPtr->GetConfigData().CameraID << std::endl;
//         cameraPtr.reset(Camera::BuildCamera(configPtr->GetConfigData().CameraID));
//     }
//     else
//     {
//         std::cout << "Using frames: " << configPtr->GetConfigData().Frames << std::endl;
//         cameraPtr.reset(Camera::BuildCamera(configPtr->GetConfigData().Frames));
//     }

//     cameraPtr->ReadFromFile(configPtr->GetConfigData().CameraConfigFile);

//     // Set the pose reader 
//     // TODO: this should also happen under the hood
//     std::vector<cv::Matx44f> gtPoses;
//     for (std::vector<float> gtPose : configPtr->GetConfigData().GroundTruthPoses)
//     {
//         float m00 = gtPose[0];
//         float m01 = gtPose[1];
//         float m02 = gtPose[2];
//         float m10 = gtPose[3];
//         float m11 = gtPose[4];
//         float m12 = gtPose[5];
//         float m20 = gtPose[6];
//         float m21 = gtPose[7];
//         float m22 = gtPose[8];
//         float m03 = gtPose[9];
//         float m13 = gtPose[10];
//         float m23 = gtPose[11];

//         gtPoses.push_back(cv::Matx44f(
//         m00, m01, m02, m03,
//         m10, m11, m12, m13,
//         m20, m21, m22, m23,
//         0, 0, 0, 1));
//     }

//     std::shared_ptr<ttool::DModelManager> modelManagerPtr = std::make_shared<ttool::DModelManager>(configPtr->GetConfigData().ModelFiles, gtPoses, configPtr);

//     // Initialize the visualizer
//     std::shared_ptr<ttool::Visualizer> visualizerPtr = std::make_shared<ttool::Visualizer>(cameraPtr, modelManagerPtr, configPtr->GetConfigData().Zn, configPtr->GetConfigData().Zf);
//     visualizerPtr->SetSaveImagePath(configPtr->GetConfigData().SaveImagePath);
//     cameraPtr->UpdateCamera();

//     ttool::Input input(modelManagerPtr, visualizerPtr);

//     // Initialize the tracker
//     tslet::ObjectTracker objectTracker;

//     // main thread
//     bool exit = false;
//     while (!exit)
//     {
//         int oid = modelManagerPtr->GetObject()->getModelID();
//         int fid = 0;
//         int key = cv::waitKey(1);

//         // Initialize the tracker with the curent object
//         objectTracker.Consume(modelManagerPtr->GetObject()->getModelID(), modelManagerPtr->GetObject(), cameraPtr->GetK());

//         // 2b UI pose input
//         std::cout << "Please input the pose of the object" << std::endl;
//         visualizerPtr->SetModels();
//         visualizerPtr->UpdateEvent(ttool::EventType::PoseInput);
//         while (oid == modelManagerPtr->GetObject()->getModelID() && !exit)
//         {
//             visualizerPtr->UpdateVisualizer(fid);
//             cameraPtr->UpdateCamera();
//             int key = cv::waitKey(1);
//             // Save the pose
//             if ('p' == key)
//             {
//                 break;
//             }
//             if (27 == key)
//             {
//                 exit = true;
//                 break;
//             }
//             objectTracker.SetPose(modelManagerPtr->GetObject()->getModelID(), input.GetPose());

//             input.ConsumeKey(key);
//         }
//         // 3 TSlet
//         visualizerPtr->UpdateEvent(ttool::EventType::Tracking);
//         objectTracker.UpdateHistogram(oid, cameraPtr->image());
//         while (oid == modelManagerPtr->GetObject()->getModelID() && !exit)
//         {
//             int key = cv::waitKey(1);

//             auto ti = cv::getTickCount();

//             if ('q' == key)
//             {
//                 break;
//             }
//             if (27 == key)
//             {
//                 exit = true;
//                 break;
//             }
//             // Break before the new object's gtID is used to estimate the pose of the previous object (snapshotted by the tracker)
//             if (oid != modelManagerPtr->GetObject()->getModelID())
//             {
//                 break;
//             }
//             objectTracker.CallEstimatePose(oid, cameraPtr->image());
//             auto tf = cv::getTickCount();
//             auto t = (tf - ti) / cv::getTickFrequency();
//             int fps = 1 / t;

//             visualizerPtr->UpdateVisualizer(fid, fps);
//             cameraPtr->UpdateCamera();
//             objectTracker.UpdateHistogram(oid, cameraPtr->image());
//             ++fid;

//             input.ConsumeKey(key);
//         }
//         oid = modelManagerPtr->GetObject()->getModelID();
//         std::cout << oid << std::endl;
//     }

//     cv::destroyAllWindows();
//     ttool::makeVideoFromAllSavedImages(configPtr->GetConfigData().SaveImagePath);

//     return 0;
// }