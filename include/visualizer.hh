#pragma once

#include "camera.hh"
#include "view.hh"
#include "viewer.hh"
#include "model.hh"
#include "d_model_manager.hh"
#include "event.hh"

namespace ttool::standaloneUtils
{
    struct Visualizer
    {
    public:
        Visualizer(std::shared_ptr<Camera> cameraPtr, std::shared_ptr<DModelManager> modelManagerPtr, float zn, float zf)
        {
            cv::Matx33f K = cameraPtr->GetK();

            View* view = View::Instance();
            view->Initialize(K, cameraPtr->GetPreprocessWidth(), cameraPtr->GetPreprocessHeight(), zn, zf, 4);

            m_ModelManagerPtr = modelManagerPtr;
            m_ModelManagerPtr->InitModels();

            m_CameraPtr = cameraPtr;

            m_ViewerPtr = std::make_shared<UnifiedViewer>();

        }

        /**
         * @brief Set the Image Save Path object
         * 
         * @param path 
         */
        void SetSaveImagePath(std::string path)
        {
            // if it doesn't exists create a folder
            if (!std::filesystem::exists(path))
            {
                std::filesystem::create_directory(path);
            }
            m_ImageSavePath = path;
        }
        
        /**
         * @brief Set the models with the current model manager
         * 
         */
        void SetModels()
        {
            View* view = View::Instance();
            m_ViewerPtr->Init("Viewer", view, m_ModelManagerPtr->GetObject(), m_CameraPtr);
        }

        /**
         * @brief Update the visualizer
         * 
         * @param frameId 
         */
        void UpdateVisualizer(int frameId, int fps = -1)
        {
            m_ViewerPtr->UpdateViewer(frameId, fps);
        }

        /**
         * @brief Toggle the show keymaps
         * 
         */
        void ToggleShowKeymaps()
        {
            m_ViewerPtr->ToggleShowKeymaps();
        }

        /**
         * @brief Update the event of the viualizer (i.e. the state of the program)
         * 
         * @param event 
         */
        void UpdateEvent(ttool::EventType event)
        {
            m_ViewerPtr->UpdateEvent(event);
        }

        /**
         * @brief Toggle the saving of images
         * 
         * This is useful for debugging, when running a program and you want to save the images which will be processed as a video later
         * 
         */
        void ToggleSavingImages()
        {
            if (m_IsSavingImages)
            {
                m_ViewerPtr->StopSavingImages();
                m_IsSavingImages = false;
            }
            else
            {
                m_IsSavingImages = true;

                std::time_t t = std::time(nullptr);
                std::tm tm = *std::localtime(&t);
                std::stringstream ss;
                ss << m_ImageSavePath << "/" << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S") << "/";
                std::string path = ss.str();

                std::filesystem::create_directory(path);
                std::filesystem::create_directory(path + "/raw/");

                m_ViewerPtr->StartSavingImages(path);
            }
        }

    private:
        std::shared_ptr<UnifiedViewer> m_ViewerPtr;
        std::shared_ptr<Camera> m_CameraPtr;
        std::shared_ptr<DModelManager> m_ModelManagerPtr;

        bool m_IsSavingImages = false;
        std::string m_ImageSavePath = "./debug/";
    };
    
}