// #include <memory>
#include <iostream>
#include <QApplication>
#include <QThread>

#include "camera.hh"
#include "ttool.hh"
#include "util.hh"

#include "pose_writer.hh"


using namespace ttool::standaloneUtils;

int main(int argc, char **argv)
{
    // parse args
    std::string helpMsg = "Usage: ./ttool \n" \
                               "[options]:\n" \
                               "[-h,--help]: print this message\n" \
                               "[-c,--camera]: camera index\n" \
                               "[-l,--calib]: calibration file for the camera\n" \
                               "[-s,--save]: save image path\n" \
                               "[-t,--trackPose]: it saves all poses and objects in a log file\n";
    int cameraID = 0;
    std::string calibFilePath;
    bool trackPose = false;
    std::string saveImagePath = "./debug/";
    if (argc == 1)
    {
        std::cout << helpMsg << std::endl;
        return 0;
    }
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            std::cout << helpMsg << std::endl;
            return 0;
        }
        else if (arg == "-c" || arg == "--camera")
        {
            if (i + 1 < argc)
            {
                cameraID = std::stoi(argv[++i]);
                std::cout << "\033[1;35m[Info]: Camera index: " << cameraID << "\033[0m" << std::endl;
            }
            else
            {
                std::cout << "\033[1;31m[Error]: camera index is not specified\033[0m" << std::endl;
                std::cout << helpMsg << std::endl;
                return 1;
            }
        }
        else if (arg == "-l" || arg == "--calib")
        {
            if (i + 1 < argc)
            {
                calibFilePath = argv[++i];
                std::cout << "\033[1;35m[Info]: Calibration file: " << calibFilePath << "\033[0m" << std::endl;
            }
            else
            {
                std::cout << "\033[1;31m[Error]: calibration file is not specified\033[0m" << std::endl;
                std::cout << helpMsg << std::endl;
                return 1;
            }
        }
        else if (arg == "-s" || arg == "--save")
        {
            if (i + 1 < argc)
            {
                saveImagePath = argv[++i];
                std::cout << "\033[1;35m[Info]: Save image path: " << saveImagePath << "\033[0m" << std::endl;
            }
            else
            {
                std::cout << "\033[1;31m[Error]: save image path is not specified\033[0m" << std::endl;
                std::cout << helpMsg << std::endl;
                return 1;
            }
        }
        else if (arg == "-t" || arg == "--trackPose")
        {
            trackPose = true;
            std::cout << "\033[1;35m[Info]: output pose recording to log\033[0m" << std::endl;
        }
        else
        {
            std::cout << "\033[1;31m[Error]: unkown argument\033[0m" << arg << std::endl;
            std::cout << helpMsg << std::endl;
            return 1;
        }
    }

    // Qt
    QApplication a(argc, argv);

    // ttool setup
    std::shared_ptr<ttool::TTool> ttool = std::make_shared<ttool::TTool>(__TTOOL_CONFIG_PATH__, calibFilePath);

    std::shared_ptr<ttool::Config> configPtr = ttool->GetConfig();

    // Set standalone console components
    std::shared_ptr<Camera> cameraPtr;
    cameraPtr.reset(Camera::BuildCamera(cameraID));
    cameraPtr->ReadFromFile(calibFilePath);

    std::shared_ptr<Visualizer> visualizerPtr = std::make_shared<Visualizer>(cameraPtr,
                                                                            ttool->GetModelManager(),
                                                                            configPtr->GetConfigData().Zn,
                                                                            configPtr->GetConfigData().Zf);
    visualizerPtr->SetSaveImagePath(saveImagePath);
    cameraPtr->UpdateCamera();
    ttool::InputVisualizer inputVisualizer(visualizerPtr);

    PoseWriter poseWriter = PoseWriter("trackingPose.log", __TTOOL_CONFIG_PATH__, configPtr->GetConfigData().ModelFiles);

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

            if (trackPose) { poseWriter.write(pose, ttool->GetCurrentObjectID()); }
        }
    }

    cv::destroyAllWindows();
    ttool::makeVideoFromAllSavedImages(saveImagePath);

}
