#include "TSegment.h"
#include "camera.hh"

#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "TEST" << std::endl;
    while (true) 
    {
        // 1 Tsegment
        auto seg = tsegment::Segmentation("/home/tpp/IBOIS/TTool/debug");
        std::shared_ptr<Camera> camera_ptr(Camera::BuildCamera(""));  // --> "" to open live stream / original: Camera::BuildCamera(gp->frames)
        while (true) {
            camera_ptr->UpdateCamera();
            cv::imshow("From Camera", camera_ptr->image());
            seg.consume_image(camera_ptr->image());
            if (seg.is_ready()) {
                // Create new thread and call -> 
                auto mask_pair = seg.get_mask();
                cv::imshow("Mask", mask_pair.second);
                cv::waitKey(0);
                break;
            }
            if (cv::waitKey(1)==27) break;

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