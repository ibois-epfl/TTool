#pragma once

#include <opencv2/opencv.hpp>

#include "object3d.hh"

namespace ttool
{
    struct DModelManager
    {
        public:
        DModelManager(std::vector<cv::String> modelFile, std::vector<std::vector<cv::Matx44f>> gtPoses)
        {
            // Initialize object(s) to with model file, starting pose, ...
            int targetFrame = 1;
            std::vector<float> distances = { 200.0f, 400.0f, 600.0f };
            int initFid = targetFrame - 1 >= 0 ? targetFrame - 1 : targetFrame;
            for (int i = 0; i < modelFile.size(); ++i)
            {
                std::shared_ptr<Object3D> objPtr = std::make_shared<Object3D>(modelFile[i],
                                                                            gtPoses[i][initFid],
                                                                            1.0,
                                                                            0.55f,
                                                                            distances);
                m_Objects.push_back(objPtr);
            }
        }

        /**
         * @brief Initialize the models
        * After view is initialized, OpenGL context is created
        * Now Model can be initialized, this should be done after view is initialized
        * because Model needs OpenGL context to initialize
        * objects should be initialized before any rendering can be done
         */
        void InitModels()
        {
            for (int i = 0; i < m_Objects.size(); ++i)
            {
                m_Objects[i]->initBuffers();
                m_Objects[i]->initialize();

                // In case of multiple objects, each object should have a unique model ID for the viewer to render the contour correctly
                // Originally in the SLET, the model ID is set in the Tracker class, but here we set it here
                m_Objects[i]->setModelID(i + 1);
                m_GroundTruthPoses[i + 1] = m_Objects[i]->getPose();
            }
        }

        /**
         * @brief Reset the object to ground truth (from a text file)
         * 
         */
        void ResetObjectToGroundTruth()
        {
            m_Objects[m_ObjectID]->setPose(m_GroundTruthPoses[m_ObjectID + 1]);
        }

        /**
         * @brief Get the object
         * 
         * @return std::shared_ptr<Object3D> 
         */
        std::shared_ptr<Object3D> GetObject()
        {
            return m_Objects[m_ObjectID];
        }

        /**
         * @brief Set the object ID
         * 
         * @param objectID 
         */
        void SetObjectID(int objectID)
        {
            m_ObjectID = objectID;
        }

        /**
         * @brief Increase the object ID
         * 
         */
        void IncreaseObjectID()
        {
            m_ObjectID = (m_ObjectID + 1) % m_Objects.size();
        }

        /**
         * @brief Get number of objects
         * 
         */
        int GetNumObjects()
        {
            return m_Objects.size();
        }

        private:
        std::vector<std::shared_ptr<Object3D>> m_Objects;
        std::map<int, cv::Matx44f> m_GroundTruthPoses;
        int m_ObjectID = 0;
    };
}