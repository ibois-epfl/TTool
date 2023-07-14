#include "d_model_manager.hh"

using namespace ttool;

DModelManager::DModelManager(std::vector<cv::String> modelFiles, std::vector<cv::Matx44f> &gtPoses, std::shared_ptr<Config> configPtr)
{
    assert(("There should be at least one model file", modelFiles.size() >= 1));
    m_ModelID2ModelFiles = ConvertVectorToMapID(modelFiles);
    m_ModelID2InitialPoses = ConvertVectorToMapID(gtPoses);
    m_ModelID2ModelPoses = ConvertVectorToMapID(gtPoses);

    SetCurrentObjectPtr();

    m_ConfigPtr = configPtr;
}

void DModelManager::InitModels()
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

void DModelManager::ResetObjectToInitialPose()
{
    m_CurrentObjectPtr->setPose(m_ModelID2InitialPoses[m_CurrentObjectID]);
    SnapshotObjectPose();
}

void DModelManager::SetObjectID(int objectID)
{
    SnapshotObjectPose();
    m_CurrentObjectID = objectID;

    // Reset the object to the initial pose
    // Clear the histogram information used by the tracker
    m_CurrentObjectPtr->reset();
    SetCurrentObjectPtr();
}

void DModelManager::IncreaseObjectID()
{
    SnapshotObjectPose();
    m_CurrentObjectID = m_CurrentObjectID % GetNumObjects() + 1;
    SetCurrentObjectPtr();
}

int DModelManager::GetNumObjects()
{
    return m_ModelID2ModelFiles.size();
}

void DModelManager::SnapshotObjectPose()
{
    m_ModelID2ModelPoses[m_CurrentObjectID] = m_CurrentObjectPtr->getPose();
}

void DModelManager::SavePosesToConfig()
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

template <typename T>
std::map<int, T> DModelManager::ConvertVectorToMapID(std::vector<T> someVector)
{
    std::map<int, T> mapID2Value;
    for (int i = 0; i < someVector.size(); ++i)
    {
        mapID2Value[i + 1] = someVector[i];
    }
    return mapID2Value;
}

/**
 * @brief Set the Current Object Ptr object to the current ID and initialize it
 * 
 */
void DModelManager::SetCurrentObjectPtr()
{
    // Initialize object(s) to with model file, starting pose, ...
    std::vector<float> distances = { 200.0f, 400.0f, 600.0f };
    m_CurrentObjectPtr = std::make_shared<Object3D>(m_ModelID2ModelFiles[m_CurrentObjectID], m_ModelID2ModelPoses[m_CurrentObjectID], 1.0, 0.55f, distances);

    InitModels();
}