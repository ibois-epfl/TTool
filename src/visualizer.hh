#pragma once

#include "global_param.hh"
#include "camera.hh"
#include "view.hh"
#include "viewer.hh"
#include "model.hh"
#include "d_model_manager.hh"

namespace ttool
{
    struct Visualizer
    {
    public:
        Visualizer(tk::GlobalParam* gp, std::shared_ptr<Camera> cameraPtr, std::shared_ptr<DModelManager> modelManagerPtr)
        {
            cv::Matx33f K = cv::Matx33f(gp->fx, 0, gp->cx, 0, gp->fy, gp->cy, 0, 0, 1);

            // Initialize the view
            View* view = View::Instance();
            view->init(K, gp->image_width, gp->image_height, gp->zn, gp->zf, 4);

            m_ModelManagerPtr = modelManagerPtr;
            m_ModelManagerPtr->InitModels();

            m_CameraPtr = cameraPtr;

            // Initialize the viewer
            m_fragmentViewerPtr = std::make_shared<FragmentViewer>();
            m_fragmentViewerPtr->Init("Fragment Viewer", view, std::vector<std::shared_ptr<Model>>(), m_CameraPtr);

            m_contourViewerPtr = std::make_shared<ContourViewer>();
            m_contourViewerPtr->Init("Contour Viewer", view, std::vector<std::shared_ptr<Model>>{m_ModelManagerPtr->GetObject()}, m_CameraPtr);
        }

        void UpdateVisualizer(int frameId)
        {
            View* view = View::Instance();
            m_fragmentViewerPtr->Init("Fragment Viewer", view, std::vector<std::shared_ptr<Model>>{m_ModelManagerPtr->GetObject()}, m_CameraPtr);
            m_contourViewerPtr->Init("Contour Viewer", view, std::vector<std::shared_ptr<Model>>{m_ModelManagerPtr->GetObject()}, m_CameraPtr);

            m_fragmentViewerPtr->UpdateViewer(frameId);
            m_contourViewerPtr->UpdateViewer(frameId);

        }

    private:
        std::shared_ptr<FragmentViewer> m_fragmentViewerPtr;
        std::shared_ptr<ContourViewer> m_contourViewerPtr;
        std::shared_ptr<Camera> m_CameraPtr;
        std::shared_ptr<DModelManager> m_ModelManagerPtr;
    };
    
}