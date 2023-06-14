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
        Visualizer(std::shared_ptr<Camera> cameraPtr, std::shared_ptr<DModelManager> modelManagerPtr, float zn, float zf)
        {
            cv::Matx33f K = cameraPtr->GetK();

            // Initialize the view
            View* view = View::Instance();
            view->init(K, cameraPtr->GetPreprocessWidth(), cameraPtr->GetPreprocessHeight(), zn, zf, 4);

            m_ModelManagerPtr = modelManagerPtr;
            m_ModelManagerPtr->InitModels();

            m_CameraPtr = cameraPtr;

            // Initialize the viewer

            m_viewerPtr = std::make_shared<UnifiedViewer>();
        }

        void SetModels()
        {
            View* view = View::Instance();
            m_viewerPtr->Init("Viewer", view, m_ModelManagerPtr->GetObject(), m_CameraPtr);
        }

        /**
         * @brief Update the visualizer
         * 
         * @param frameId 
         */
        void UpdateVisualizer(int frameId, int fps = -1)
        {
            m_viewerPtr->UpdateViewer(frameId, fps);
        }

        void ToggleShowKeymaps()
        {
            m_viewerPtr->ToggleShowKeymaps();
        }

        void UpdateEvent(ttool::EventType event)
        {
            m_viewerPtr->UpdateEvent(event);
        }

        void ToggleSavingImages(std::string path)
        {
            if (m_IsSavingImages)
            {
                m_viewerPtr->StopSavingImages();
            }
            else
            {
                m_viewerPtr->StartSavingImages(path);
            }
        }

    private:
        std::shared_ptr<UnifiedViewer> m_viewerPtr;
        std::shared_ptr<Camera> m_CameraPtr;
        std::shared_ptr<DModelManager> m_ModelManagerPtr;
        bool m_IsSavingImages = false;
    };
    
}