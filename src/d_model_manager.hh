#pragma once

#include "object3d.hh"
#include "config.hh"

#include <opencv2/opencv.hpp>


namespace ttool
{
    struct DModelManager
    {
        public:
        DModelManager(std::vector<cv::String> modelFile, std::vector<cv::Matx44f> &gtPoses, std::shared_ptr<Config> configPtr)
        {
            // Initialize object(s) to with model file, starting pose, ...
            std::vector<float> distances = { 200.0f, 400.0f, 600.0f };
            for (int i = 0; i < modelFile.size(); ++i)
            {
                std::cout << modelFile[i] << std::endl;
                std::cout << gtPoses[i] << std::endl;
                std::shared_ptr<Object3D> objPtr = std::make_shared<Object3D>(modelFile[i],
                                                                            gtPoses[i],
                                                                            1.0,
                                                                            0.55f,
                                                                            distances);
                m_Objects.push_back(objPtr);
            }

            m_ConfigPtr = configPtr;
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

        /**
         * @brief Save all the poses of the model to the specified config file (m_PoseOutput)
         * 
         */
        void SavePosesToConfig()
        {
            std::vector<std::vector<float>> poses;
            for (int i = 0; i < m_Objects.size(); ++i)
            {
                cv::Matx44f pose = m_Objects[i]->getPose();
                float m00 = pose(0, 0);
                float m01 = pose(0, 1);
                float m02 = pose(0, 2);
                float m03 = pose(0, 3); 
                float m10 = pose(1, 0);
                float m11 = pose(1, 1);
                float m12 = pose(1, 2);
                float m13 = pose(1, 3);
                float m20 = pose(2, 0);
                float m21 = pose(2, 1);
                float m22 = pose(2, 2);
                float m23 = pose(2, 3);


                poses.push_back(
                    std::vector<float>{m00, m01, m02,
                                       m10, m11, m12,
                                       m20, m21, m22,
                                       m03, m13, m23});
            }
            
            std::cout << m_PoseOutput << std::endl;
            
            m_ConfigPtr->write("groundTruthPoses", poses);
        }

        private:
        std::string m_PoseOutput;
        std::vector<std::shared_ptr<Object3D>> m_Objects;
        std::map<int, cv::Matx44f> m_GroundTruthPoses;
        int m_ObjectID = 0;
        std::shared_ptr<Config> m_ConfigPtr;
    };
}