#pragma once

#include "camera.hh"
#include "view.hh"
#include "viewer.hh"
#include "model.hh"
#include "d_model_manager.hh"
#include "event.hh"

namespace ttool
{
    struct Visualizer
    {
    public:
        Visualizer(std::shared_ptr<Camera> cameraPtr, std::shared_ptr<DModelManager> modelManagerPtr, int zn, int zf)
        {
            cv::Matx33f K = cameraPtr->GetK();

            // Initialize the view
            View* view = View::Instance();
            view->init(K, cameraPtr->GetPreprocessWidth(), cameraPtr->GetPreprocessHeight(), zn, zf, 4);

            m_ModelManagerPtr = modelManagerPtr;
            m_ModelManagerPtr->InitModels();

            m_CameraPtr = cameraPtr;

            // Initialize the viewer
            m_fragmentViewerPtr = std::make_shared<FragmentViewer>();
            m_fragmentViewerPtr->Init("Fragment Viewer", view, std::vector<std::shared_ptr<Model>>(), m_CameraPtr);

            m_contourViewerPtr = std::make_shared<ContourViewer>();
            m_contourViewerPtr->Init("Contour Viewer", view, std::vector<std::shared_ptr<Model>>(), m_CameraPtr);
        }

        void SetModels()
        {
            View* view = View::Instance();
            m_fragmentViewerPtr->Init("Fragment Viewer", view, std::vector<std::shared_ptr<Model>>{m_ModelManagerPtr->GetObject()}, m_CameraPtr);
            m_contourViewerPtr->Init("Contour Viewer", view, std::vector<std::shared_ptr<Model>>{m_ModelManagerPtr->GetObject()}, m_CameraPtr);
        }

        /**
         * @brief Update the visualizer
         * 
         * @param frameId 
         */
        void UpdateVisualizer(int frameId, int fps = 69)
        {
            m_fragmentViewerPtr->UpdateViewer(frameId, fps);
            m_contourViewerPtr->UpdateViewer(frameId, fps);
        }

        void ToggleShowKeymaps()
        {
            m_fragmentViewerPtr->ToggleShowKeymaps();
            m_contourViewerPtr->ToggleShowKeymaps();
        }

        void UpdateEvent(ttool::EventType event)
        {
            m_fragmentViewerPtr->UpdateEvent(event);
            m_contourViewerPtr->UpdateEvent(event);
        }

    private:
        std::shared_ptr<FragmentViewer> m_fragmentViewerPtr;
        std::shared_ptr<ContourViewer> m_contourViewerPtr;
        std::shared_ptr<Camera> m_CameraPtr;
        std::shared_ptr<DModelManager> m_ModelManagerPtr;
    };
    
}