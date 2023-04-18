#pragma once

#include "global_param.hh"
#include "camera.hh"
#include "view.hh"
#include "viewer.hh"
#include "model.hh"

namespace ttool
{
    struct Visualizer
    {
    public:
        Visualizer(tk::GlobalParam* gp, std::shared_ptr<Camera> cameraPtr, const std::vector<std::shared_ptr<Model>> &objects)
        {
            m_Objects = objects;

            cv::Matx33f K = cv::Matx33f(gp->fx, 0, gp->cx, 0, gp->fy, gp->cy, 0, 0, 1);

            // Initialize the view
            View* view = View::Instance();
            view->init(K, gp->image_width, gp->image_height, gp->zn, gp->zf, 4);

            // After view is initialized, OpenGL context is created
            // Now Model can be initialized, this should be done after view is initialized
            // because Model needs OpenGL context to initialize
            // objects should be initialized before any rendering can be done
            for (int i = 0; i < objects.size(); ++i)
            {
                objects[i]->initBuffers();
                objects[i]->initialize();

                // In case of multiple objects, each object should have a unique model ID for the viewer to render the contour correctly
                // Originally in the SLET, the model ID is set in the Tracker class, but here we set it here
                objects[i]->setModelID(i + 1);
            }

            // Initialize the viewer
            m_fragmentViewerPtr = std::make_shared<FragmentViewer>();
            m_fragmentViewerPtr->Init("Fragment Viewer", view, std::vector<std::shared_ptr<Model>>(objects.begin(), objects.end()), cameraPtr);

            m_contourViewerPtr = std::make_shared<ContourViewer>();
            m_contourViewerPtr->Init("Contour Viewer", view, std::vector<std::shared_ptr<Model>>(objects.begin(), objects.end()), cameraPtr);
        }

        void UpdateVisualizer(int frameId)
        {
            m_fragmentViewerPtr->UpdateViewer(frameId);
            m_contourViewerPtr->UpdateViewer(frameId);
        }

    private:
        std::vector<std::shared_ptr<Model>> m_Objects;
        std::shared_ptr<FragmentViewer> m_fragmentViewerPtr;
        std::shared_ptr<ContourViewer> m_contourViewerPtr;
    };
    
}