#include "TSegment.h"
#include "camera.hh"

#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "TEST" << std::endl;

    auto seg = tsegment::Segmentation("");
    std::shared_ptr<Camera> camera_ptr(Camera::BuildCamera(""));  // --> "" to open live stream / original: Camera::BuildCamera(gp->frames)
    while (true) {
        camera_ptr->UpdateCamera();
        cv::imshow("From Camera", camera_ptr->image());
        seg.consume_image(camera_ptr->image());
        if (cv::waitKey(1)==27) break;
    }
    auto mask_pair = seg.get_mask();
    if (mask_pair.first) {
        cv::imshow("Mask", mask_pair.second);
        cv::waitKey(0);
    }

    return 0;
}