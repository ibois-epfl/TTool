#include <memory>
#include <iostream>
#include <QApplication>
#include <QThread>

#include "camera.hh"
#include "ttool.hh"

int main(int argc, char **argv)
{


    std::shared_ptr<ttool::TTool> ttool = std::make_shared<ttool::TTool>(__TTOOL_CONFIG_PATH__);

    ttool->GetConfig();

    QApplication a(argc, argv);
    std::shared_ptr<Camera> cameraPtr;

    // instantiate the visualizer here as standalone utility

    // TODO:
    // the ttool needs:
    // - a) it needs index of the model
    // - b) it needs the frame
    // - c) it needs the initial/modified pose
    // after we can start tracking


}