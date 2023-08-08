#pragma once

#include "object3d.hh"
#include "config.hh"

#include <opencv2/opencv.hpp>


namespace ttool
{
    /**
     * @brief Class to manage the models, all other classes should use this class to access the models
     * The current model can be accessed via GetObject()
     * 
     */
    struct DModelManager
    {
        public:
        /**
         * @brief Construct a new DModelManager object
         * 
         * @param modelFiles list of path to the model files
         * @param gtPoses    list of ground truth poses
         * @param configPtr  pointer to the config
         */
        DModelManager(std::vector<cv::String> modelFiles, std::vector<cv::Matx44f> &gtPoses, std::shared_ptr<Config> configPtr);

        /**
         * @brief Initialize the models
        * After view is initialized, OpenGL context is created
        * this function should be called after view is initialized
        * because Model needs OpenGL context to initialize
        * objects should be initialized before any rendering can be done
        * 
        */
        void InitModels();

        /**
         * @brief Reset the object to ground truth (from a text file)
         * 
         */
        void ResetObjectToConfigInitialPose();

        /**
         * @brief Save the pose as model initial pose as well as keep the metadata on the model manager
         * 
         * @param initialPose the new initial pose to be save to the model and the mdoel manager
        */
       void SaveObjectInitialPose(cv::Matx44f initialPose);
        /**
         * @brief Reset the object to the initial pose (which may be saved by the user during the pose adjustment)
         * 
        */
        void ResetObjectToLastSavePose();

        /**
         * @brief Get the current object
         * 
         * @return std::shared_ptr<Object3D> 
         */
        std::shared_ptr<Object3D> GetObject() { return m_CurrentObjectPtr; }

        /**
         * @brief Get the object name
         * 
         * @return std::string 
         */
        std::string GetObjectName() { return m_ModelID2ModelName[m_CurrentObjectID]; }

        /**
         * @brief Set the object ID
         * 
         * @param objectID 
         */
        void SetObjectID(int objectID);

        /**
         * @brief Increase the object ID
         * 
         */
        void IncreaseObjectID();

        /**
         * @brief Get number of objects
         * 
         */
        int GetNumObjects();

        /**
         * @brief Snapshot the current pose of the current object
         * Because pose is in the object, but we want that to be in sync with the model manager
         * 
         */
        void SnapshotObjectPose();

        /**
         * @brief Save all the poses of the model to the specified config file (m_PoseOutput)
         * 
         */
        void SavePosesToConfig();

        /**
         * @brief Get the Object ID object from the model name according to the folder name in the config file
         * 
         * @param modelName 
         * @return int 
         */
        int GetObjectID(std::string modelName);

        private:
        /**
         * @brief Convert a vector to a map with ID as key
         * The ID is 1 based
         * 
         * @tparam T 
         * @param someVector 
         * @return std::map<int, T>  map with ID as key
         */
        template <typename T>
        std::map<int, T> ConvertVectorToMapID(std::vector<T> someVector);

        /**
         * @brief Convert a vector to a map with string as key and model ID as value
         * 
         * @param someVector
         * @return std::map<std::string, int> map with string as key and model ID as value
        */
        std::map<std::string, int> ConvertVectorToMapString(std::vector<cv::String> someVector);

        
        /**
         * @brief Make a map with model ID as key and model name as value
         * 
         * @param modelFiles 
         * @return std::map<int, std::string> 
         */
        std::map<int, std::string> MakeModelID2ModelName(std::vector<cv::String> modelFiles);

        /**
         * @brief Set the Current Object Ptr object to the current ID and initialize it
         * 
         */
        void SetCurrentObjectPtr();

        private:
        std::map<int, std::string> m_ModelID2ModelFiles;
        std::map<int, cv::Matx44f> m_ModelID2InitialPoses;
        std::map<int, cv::Matx44f> m_ModelID2ModelPoses;
        std::map<int, cv::Matx44f> m_ModelID2LastSavedPoses;

        std::map<std::string, int> m_ModelName2ModelID;
        std::map<int, std::string> m_ModelID2ModelName;

        std::shared_ptr<Config> m_ConfigPtr;

        int m_CurrentObjectID = 1; // 1 based indexing
        std::shared_ptr<Object3D> m_CurrentObjectPtr;
    };
}