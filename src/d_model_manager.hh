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
            assert(("There should be at least one model file", modelFile.size() >= 1));
            m_ModelID2ModelFiles = convertVectorToMapID(modelFile);
            m_ModelID2GroundTruthPoses = convertVectorToMapID(gtPoses);
            m_ModelID2ModelPoses = convertVectorToMapID(gtPoses);

            setCurrentObjectPtr();

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
            if (m_CurrentObjectPtr->isInitialized())
            {
                return;
            }
            m_CurrentObjectPtr->initBuffers();
            m_CurrentObjectPtr->initialize(); // mark Model as initialized

            // In case of multiple objects, each object should have a unique model ID for the viewer to render the contour correctly
            // Originally in the SLET, the model ID is set in the Tracker class, but here we set it here
            m_CurrentObjectPtr->setModelID(m_CurrentObjectID);
        }

        /**
         * @brief Reset the object to ground truth (from a text file)
         * 
         */
        void ResetObjectToGroundTruth()
        {
            m_CurrentObjectPtr->setPose(m_ModelID2GroundTruthPoses[m_CurrentObjectID]);
            SnapshotObjectPose();
        }

        /**
         * @brief Get the object
         * 
         * @return std::shared_ptr<Object3D> 
         */
        std::shared_ptr<Object3D> GetObject()
        {
            return m_CurrentObjectPtr;
        }

        /**
         * @brief Set the object ID
         * 
         * @param objectID 
         */
        void SetObjectID(int objectID)
        {
            SnapshotObjectPose();
            m_CurrentObjectID = objectID;
            setCurrentObjectPtr();
        }

        /**
         * @brief Increase the object ID
         * 
         */
        void IncreaseObjectID()
        {
            SnapshotObjectPose();
            m_CurrentObjectID = m_CurrentObjectID % GetNumObjects() + 1;
            setCurrentObjectPtr();
        }

        /**
         * @brief Get number of objects
         * 
         */
        int GetNumObjects()
        {
            return m_ModelID2ModelFiles.size();
        }

        /**
         * @brief Snapshot the current pose of the current object
         * 
         */
        void SnapshotObjectPose()
        {
            m_ModelID2ModelPoses[m_CurrentObjectID] = m_CurrentObjectPtr->getPose();
        }

        /**
         * @brief Save all the poses of the model to the specified config file (m_PoseOutput)
         * 
         */
        void SavePosesToConfig()
        {
            SnapshotObjectPose();

            std::vector<std::vector<float>> poses;
            for (int id = 1; id <= GetNumObjects(); ++id)
            {
                cv::Matx44f pose = m_ModelID2ModelPoses[id];
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
            
            m_ConfigPtr->write("groundTruthPoses", poses);
        }

        private:
        template <typename T>
        std::map<int, T> convertVectorToMapID(std::vector<T> someVector)
        {
            std::map<int, T> mapID2Value;
            for (int i = 0; i < someVector.size(); ++i)
            {
                mapID2Value[i + 1] = someVector[i];
            }
            return mapID2Value;
        }

        void setCurrentObjectPtr()
        {
            // Initialize object(s) to with model file, starting pose, ...
            std::vector<float> distances = { 200.0f, 400.0f, 600.0f };
            m_CurrentObjectPtr = std::make_shared<Object3D>(m_ModelID2ModelFiles[m_CurrentObjectID], m_ModelID2ModelPoses[m_CurrentObjectID], 1.0, 0.55f, distances);

            InitModels();
        }

        private:
        std::map<int, std::string> m_ModelID2ModelFiles;
        std::map<int, cv::Matx44f> m_ModelID2GroundTruthPoses;
        std::map<int, cv::Matx44f> m_ModelID2ModelPoses;

        std::shared_ptr<Config> m_ConfigPtr;

        int m_CurrentObjectID = 1; // 1 based indexing
        std::shared_ptr<Object3D> m_CurrentObjectPtr;
    };
}