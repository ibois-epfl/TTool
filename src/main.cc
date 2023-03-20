#include "TSegment.h"
#include "camera.hh"
#include "viewer.hh"
#include "view.hh"
#include "global_param.hh"
#include "model.hh" // Just for testing

#include <iostream>
#include <QApplication>
#include <QThread>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    std::string config_file = "/home/tpp/IBOIS/TTool/model_drill_scaled/config.yml";

    tk::GlobalParam* gp = tk::GlobalParam::Instance();
	gp->ParseConfig(config_file);

    std::cout << "Frames: " << gp->frames << std::endl;
	std::shared_ptr<Camera> camera_ptr(Camera::BuildCamera(gp->frames));  // --> "" to open live stream / original: Camera::BuildCamera(gp->frames)
	gp->image_width = camera_ptr->width;
	gp->image_height = camera_ptr->height;
    std::cout << "Camera initialized" << std::endl;
    cv::Matx33f K = cv::Matx33f(gp->fx, 0, gp->cx, 0, gp->fy, gp->cy, 0, 0, 1);
 
    //  Just for testing
    std::vector<Model*> objects;

    // Initialize the view
	View* view = View::Instance();
    std::cout << "view: " << view << std::endl;  // --> DEBUG
    std::cout << "View instance created" << std::endl;
    std::cout << "image_width: " << gp->image_width << " image_height: " << gp->image_height << std::endl;
	view->init(K, gp->image_width, gp->image_height, gp->zn, gp->zf, 4);
    std::cout << "View initialized" << std::endl;

    // Initialize the viewer
    std::shared_ptr<FragmentViewer> fragment_viewer_ptr_ = std::make_shared<FragmentViewer>();
	fragment_viewer_ptr_->Init("fragment viewer", view, std::vector<Model*>(objects.begin(), objects.end()), camera_ptr);
    std::cout << "Fragment viewer initialized" << std::endl;
    
    while (true)
    {
        // 1 Tsegment
        std::string dubugPath = "/home/tpp/IBOIS/TTool/debug";
        auto seg = tsegment::Segmentation("");
        int fid = 0;
        while (true)
        {
            camera_ptr->UpdateCamera();
            fragment_viewer_ptr_->UpdateViewer(fid);
            fid++;
            seg.ConsumeImage(camera_ptr->image());
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