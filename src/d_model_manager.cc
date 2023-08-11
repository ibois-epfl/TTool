/**
 * TTool
 * Copyright (C) 2023  Andrea Settimi, Naravich Chutisilp (IBOIS, EPFL)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "d_model_manager.hh"

ttool::DModelManager::DModelManager(std::vector<cv::String> modelFiles, std::vector<cv::Matx44f> &gtPoses, std::shared_ptr<Config> configPtr)
{
    assert(("There should be at least one model file", modelFiles.size() >= 1));
    m_ModelID2ModelFiles = ConvertVectorToMapID(modelFiles);
    m_ModelID2InitialPoses = ConvertVectorToMapID(gtPoses);
    m_ModelID2ModelPoses = ConvertVectorToMapID(gtPoses);
    m_ModelID2LastSavedPoses = ConvertVectorToMapID(gtPoses);
    m_ModelName2ModelID = ConvertVectorToMapString(modelFiles);
    m_ModelID2ModelName = MakeModelID2ModelName(modelFiles);

    SetCurrentObjectPtr();

    m_ConfigPtr = configPtr;
}

void ttool::DModelManager::InitModels()
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

void ttool::DModelManager::ResetObjectToConfigInitialPose()
{
    m_CurrentObjectPtr->setPose(m_ModelID2InitialPoses[m_CurrentObjectID]);
    SnapshotObjectPose();
}

void ttool::DModelManager::SaveObjectInitialPose(cv::Matx44f initialPose)
{
    m_ModelID2LastSavedPoses[m_CurrentObjectID] = initialPose;
    SnapshotObjectPose();
}

void ttool::DModelManager::ResetObjectToLastSavePose()
{
    m_CurrentObjectPtr->setPose(m_ModelID2LastSavedPoses[m_CurrentObjectID]);
    SnapshotObjectPose();
}

void ttool::DModelManager::SetObjectID(int objectID)
{
    SnapshotObjectPose();
    m_CurrentObjectID = objectID;

    // Reset the object to the initial pose
    // Clear the histogram information used by the tracker
    m_CurrentObjectPtr->reset();
    SetCurrentObjectPtr();
}

void ttool::DModelManager::IncreaseObjectID()
{
    SnapshotObjectPose();
    m_CurrentObjectID = m_CurrentObjectID % GetNumObjects() + 1;
    SetCurrentObjectPtr();
}

int ttool::DModelManager::GetNumObjects()
{
    return m_ModelID2ModelFiles.size();
}

void ttool::DModelManager::SnapshotObjectPose()
{
    m_ModelID2ModelPoses[m_CurrentObjectID] = m_CurrentObjectPtr->getPose();
}

void ttool::DModelManager::SavePosesToConfig()
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

int ttool::DModelManager::GetObjectID(std::string modelName)
{ 
    if (m_ModelName2ModelID.find(modelName) != m_ModelName2ModelID.end())
        return m_ModelName2ModelID[modelName]; 
    return -1;
}

template <typename T>
std::map<int, T> ttool::DModelManager::ConvertVectorToMapID(std::vector<T> someVector)
{
    std::map<int, T> mapID2Value;
    for (int i = 0; i < someVector.size(); ++i)
    {
        mapID2Value[i + 1] = someVector[i];
    }
    return mapID2Value;
}

std::map<std::string, int> ttool::DModelManager::ConvertVectorToMapString(std::vector<cv::String> someVector)
{
    std::map<std::string, int> mapString2ID;
    for (int i = 0; i < someVector.size(); ++i)
    {
        std::string fullpath = someVector[i];

        std::string folderName = fullpath.substr(0, fullpath.find_last_of("/\\"));
        std::string modelName = folderName.substr(folderName.find_last_of("/\\") + 1);
        // std::cout << modelName << std::endl;
        mapString2ID[modelName] = i + 1;
    }
    return mapString2ID;
}

std::map<int, std::string> ttool::DModelManager::MakeModelID2ModelName(std::vector<cv::String> someVector)
{
    std::map<int, std::string> mapID2String;
    for (int i = 0; i < someVector.size(); ++i)
    {
        std::string fullpath = someVector[i];

        std::string folderName = fullpath.substr(0, fullpath.find_last_of("/\\"));
        std::string modelName = folderName.substr(folderName.find_last_of("/\\") + 1);
        // std::cout << modelName << std::endl;
        mapID2String[i + 1] = modelName;
    }
    return mapID2String;
}

/**
 * @brief Set the Current Object Ptr object to the current ID and initialize it
 * 
 */
void ttool::DModelManager::SetCurrentObjectPtr()
{
    // Initialize object(s) to with model file, starting pose, ...
    std::vector<float> distances = { 200.0f, 400.0f, 600.0f };
    m_CurrentObjectPtr = std::make_shared<ttool::tslet::Object3D>(m_ModelID2ModelFiles[m_CurrentObjectID], m_ModelID2ModelPoses[m_CurrentObjectID], 1.0, 0.55f, distances);

    InitModels();
}