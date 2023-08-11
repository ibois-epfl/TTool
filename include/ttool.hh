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

#ifndef __TTOOL_H__
#define __TTOOL_H__

#include <string>

#include "ttool_exports.hh"
#include "d_model_manager.hh"
#include "input.hh"
#include "object_tracker.hh"
#include "object3d.hh"
#include "view.hh"
#include "config.hh"
#include "classifier.hh"
#include "pose_writer.hh"

namespace ttool
{
    /**
     * @brief This class is the main class of the TTool library.
     * 
     */
    class TTOOL_API TTool
    {
    public:
        TTool(std::string ttoolRootPath, std::string configFile, std::string cameraCalibFile)
        {
            InitializeConfig(ttoolRootPath, configFile);

            m_CameraCalibFile = cameraCalibFile;

            InitializeClassifier();
            
            // Initialize the camera matrix from the camera calibration file, as well as the camera size (width and height)
            ReadCameraMatrix();
            
            InitializeView();

            std::vector<cv::Matx44f> preprocessedGroundTruthPoses = PreprocessGroundTruthPoses(m_ConfigPtr->GetConfigData().GroundTruthPoses);

            m_ModelManagerPtr = std::make_shared<ttool::DModelManager>(
                m_ConfigPtr->GetConfigData().ModelFiles,
                preprocessedGroundTruthPoses,
                m_ConfigPtr);

            m_Input = ttool::InputModelManager(m_ModelManagerPtr);
            InitializeObjectTracker();

            // dry run to initialize silouhette drawing
            cv::Mat emptyMat = cv::Mat::zeros(480, 640, CV_8UC3);
            RunOnAFrame(emptyMat);
        }

        TTool(std::string configFile, std::string cameraCalibFile)
        {
            InitializeConfig("", configFile);

            m_CameraCalibFile = cameraCalibFile;
            
            InitializeClassifier();

            // Initialize the camera matrix from the camera calibration file, as well as the camera size (width and height)
            ReadCameraMatrix();
            
            InitializeView();

            std::vector<cv::Matx44f> preprocessedGroundTruthPoses = PreprocessGroundTruthPoses(m_ConfigPtr->GetConfigData().GroundTruthPoses);

            m_ModelManagerPtr = std::make_shared<ttool::DModelManager>(
                m_ConfigPtr->GetConfigData().ModelFiles,
                preprocessedGroundTruthPoses,
                m_ConfigPtr);

            m_Input = ttool::InputModelManager(m_ModelManagerPtr);
            InitializeObjectTracker();

            // dry run to initialize silouhette drawing
            cv::Mat emptyMat = cv::Mat::zeros(480, 640, CV_8UC3);
            RunOnAFrame(emptyMat);
            // DrawSilhouette(emptyMat);
            CheckObjectChange();
        };

        TTool(std::string ttoolRootPath, std::string configFile, cv::Mat cameraMatrix, cv::Size cameraSize)
        {
            InitializeConfig(ttoolRootPath, configFile);

            InitializeClassifier();

            CameraMatrix = cameraMatrix.clone();
            CamSize.height = cameraSize.height;
            CamSize.width = cameraSize.width;

            InitializeView();

            std::vector<cv::Matx44f> preprocessedGroundTruthPoses = PreprocessGroundTruthPoses(m_ConfigPtr->GetConfigData().GroundTruthPoses);

            m_ModelManagerPtr = std::make_shared<ttool::DModelManager>(
                m_ConfigPtr->GetConfigData().ModelFiles,
                preprocessedGroundTruthPoses,
                m_ConfigPtr);

            m_Input = ttool::InputModelManager(m_ModelManagerPtr);
            InitializeObjectTracker();
            CheckObjectChange();
        };

        ~TTool(){};

        /**
         * @brief Manipulate the model based on the key press
         * This include changing the model index and adjusting the initial pose
         * of the model
         * 
         * @param key translate (w, s, a, d, q, e) or rotate (i, k, j, l, u, o)
         */
        void ManipulateModel(char key)
        {
            m_Input.ConsumeKey(key);

            // When m_Input changes the object ID, we need to update the object tracker
            CheckObjectChange();

            // When m_Input changes the object pose, we need to update the object tracker
            m_ObjectTracker.SetPose(m_CurrentObjectID, m_Input.GetPose());
        }

        /**
         * @brief Set the Model Manipulation Translation Scale object
         * 
         * @param scale 
         */
        void SetModelManipulationTranslationScale(float scale)
        {
            m_Input.SetTranslationScale(scale);
        }
        
        /**
         * @brief Set the Model Manipulation Rotation Scale object
         * 
         * @param scale 
         */
        void SetModelManipulationRotationScale(float scale)
        {
            m_Input.SetRotationScale(scale);
        }

        /**
         * @brief Run the object tracker on a frame
         * 
         * @param frame 
         */
        void RunOnAFrame(cv::Mat frame)
        {
            CheckObjectChange();
            m_ObjectTracker.UpdateHistogram(m_ModelManagerPtr->GetObject(), m_CurrentObjectID, frame);
            m_ObjectTracker.CallEstimatePose(m_ModelManagerPtr->GetObject(), m_CurrentObjectID, frame);
        }

        /**
         * @brief Classify the tool based on the camera frame
         * 
         * @param frame 
         * @return std::string the name of the tool
         */
        std::string Classify(cv::Mat frame)
        {
            int prediction = m_Classifier->Classify(frame);
            return m_Classifier->GetLabel(prediction);
        }

        /**
         * @brief Get the Classifier Log object
         * 
         * @return std::string 
         */
        std::string GetClassifierLog()
        {
            return m_Classifier->ClassifierLog.str();
        }

        /**
         * @brief Get the top k labels of the classification of the image frame of a tool head
         * 
         * @param frame 
         * @return std::vector<std::string> 
         */
        std::vector<std::string> ClassifyWithSortedLabels(cv::Mat frame)
        {
            return m_Classifier->ClassifyTopK(frame, m_Classifier->GetTotalClass());

        }

        /**
         * @brief Draw the silhouette of the model on the camera frame
         * 
         * @param frame camera frame
         * @param clr color of the silhouette
         */
        void DrawSilhouette(cv::Mat& frame, glm::vec3 clr = glm::vec3(97.0f, 48.0f, 225.0f))
        {
            View *view = View::Instance();

            view->RenderSilhouette(m_ModelManagerPtr->GetObject(), GL_FILL);

            cv::Mat depth = view->DownloadFrame(View::DEPTH); // This is the depth map of the model 0.0f and 1.0f are the min and max depth

            float alpha = 0.5f;
            for (int y = 0; y < frame.rows; y++)
            {
                for (int x = 0; x < frame.cols; x++)
                {
                    cv::Vec3b color = cv::Vec3b(clr.x, clr.y, clr.z);
                    if (depth.at<float>(y, x) != 0.0f)
                    {
                        frame.at<cv::Vec3b>(y, x) = alpha * color + (1.0f - alpha) * frame.at<cv::Vec3b>(y, x);
                    }
                }
            }

            cv::Mat maskCvt;
            view->ConvertMask(depth, maskCvt, m_ModelManagerPtr->GetObject()->getModelID());
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(maskCvt, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            cv::Vec3b color = cv::Vec3b(30, 255, 0);
            for (int i = 0; i < contours.size(); i++)
                cv::drawContours(frame, contours, i, color, 2);
        }

        /**
         * @brief Draw the shaded of the model on the camera frame
         * 
         * @param frame camera frame
         * @param clr color of the silhouette
         */
        void DrawShaded(cv::Mat& frame)
        {
            View *view = View::Instance();

            view->RenderSilhouette(m_ModelManagerPtr->GetObject(), GL_FILL);

            cv::Mat depth = view->DownloadFrame(View::DEPTH); // This is the depth map of the model 0.0f and 1.0f are the min and max depth

            view->RenderShaded(m_ModelManagerPtr->GetObject(), GL_FILL);
            cv::Mat rgb = view->DownloadFrame(View::RGB); // This is the rendered image of the model

            float alpha = 0.5f;
            for (int y = 0; y < frame.rows; y++)
            {
                for (int x = 0; x < frame.cols; x++)
                {
                    if (depth.at<float>(y, x) != 0.0f)
                    {
                        frame.at<cv::Vec3b>(y, x) = alpha * rgb.at<cv::Vec3b>(y, x) + (1.0f - alpha) * frame.at<cv::Vec3b>(y, x);
                    }
                }
            }

            cv::Mat maskCvt;
            view->ConvertMask(depth, maskCvt, m_ModelManagerPtr->GetObject()->getModelID());
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(maskCvt, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            cv::Vec3b color = cv::Vec3b(30, 255, 0);
            for (int i = 0; i < contours.size(); i++)
                cv::drawContours(frame, contours, i, color, 2);
        }

        /// @brief Destroy the View object
        void DestrolView()
        {
            View *view = View::Instance();
            view->Destroy();
        }

        /**
         * @brief Get the Pose object
         * 
         * @return cv::Matx44f 
         */
        cv::Matx44f GetPose()
        {
            CheckObjectChange();
            return m_ModelManagerPtr->GetObject()->getPose();
        }
        /**
         * @brief Get the Model object
         * 
         * @return std::shared_ptr<ttool::Model> 
         */
        std::shared_ptr<ttool::tslet::Object3D> GetModel()
        {
            CheckObjectChange();
            return m_ModelManagerPtr->GetObject();
        }

        /**
         * @brief Set the Object ID with its id
         * 
         * @param objectID the id of the object given by the index of its position in the config ttool file
         */
        void SetObjectID(int objectID)
        {
            m_CurrentObjectID = objectID;
            m_ModelManagerPtr->SetObjectID(objectID);
            InitializeObjectTracker();
        }

        // /**
        //  * @brief Set the Object ID with its name
        //  * 
        //  * @param objectName the name of the object given by the name of its position in the config ttool file
        //  */
        // void SetObjectName(std::string objectName)
        // {
        //     m_CurrentObjectID = m_ConfigPtr->GetConfigData().ObjectIDMap[objectName];
        //     m_ModelManagerPtr->SetObjectID(m_CurrentObjectID);
        //     InitializeObjectTracker();
        // }

        /// @brief Start the object tracker
        void InitializeObjectTracker()
        {
            m_ObjectTracker.Consume(m_ModelManagerPtr->GetObject()->getModelID(), m_ModelManagerPtr->GetObject(), CameraMatrix);
        }
        /// @brief Initialize the view
        void InitializeView()
        {
            // Initialize the view
            View* view = View::Instance();
            float zn = m_ConfigPtr->GetConfigData().Zn;
            float zf = m_ConfigPtr->GetConfigData().Zf;
            view->Initialize(CameraMatrix, CamSize.width, CamSize.height, zn, zf, 4);
        }

        std::string GetTrackingStatus()
        {
            return m_ObjectTracker.GetTrackingStatus();
        }

        /**
         * @brief Write the pose of the object to a file with a corresponding image
         * 
         * @param image 
         */
        void WritePoseToFile(cv::Mat image)
        {
            if (m_PoseWriter == nullptr)
            {
                std::time_t time = std::time({});
                char timeString[std::size("YYYY-MM-DD_HH-MM-SS")];
                std::strftime(timeString, sizeof(timeString), "%Y-%m-%d_%H-%M-%S", std::localtime(&time));

                std::string outputParentDir = m_ConfigPtr->GetTToolRootPath() + "/TToolOutput/";

                std::string outputDir = outputParentDir + timeString;
                std::filesystem::create_directories(outputDir);
                
                std::string outputLogFile = outputDir + "/PoseLog.txt";

                std::filesystem::create_directory(outputDir);

                m_PoseWriter = std::make_unique<ttool::PoseWriter>(outputLogFile, m_ConfigFile, m_ConfigPtr->GetConfigData().ModelFiles);
                
                // Make directory for the images, create "TToolOutput/<CurrentDate>__<CurrentTime>" directory
                m_PoseWriter->SetImageDir(outputDir);
            }
            auto pose = GetPose();
            m_PoseWriter->Write(pose, GetCurrentObjectID(), m_ModelManagerPtr->GetObjectName(), image);
        }

        /**
         * @brief Reset the pose writer
         * This will make the pose writer to create a new directory for the next pose writing
         * 
         */
        void ResetPoseWriter() { m_PoseWriter = nullptr; }

    public:
        std::shared_ptr<ttool::Config> GetConfig() { return m_ConfigPtr; };
        std::shared_ptr<ttool::DModelManager> GetModelManager() { return m_ModelManagerPtr; };
        int GetCurrentObjectID() { return m_CurrentObjectID; };

    private:
        /**
         * @brief Initialize the config class with the TTool Root Path and the config file
         * 
         * @param ttoolRootPath path to the TTool root directory
         * @param configFile    path to the config file
         */
        void InitializeConfig(std::string ttoolRootPath, std::string configFile)
        {
            m_ConfigFile = configFile;
            m_ConfigPtr = std::make_shared<ttool::Config>(configFile);
            m_ConfigPtr->SetTToolRootPath(ttoolRootPath);
        }

        /**
         * @brief Initialize the tool head classifier
         * 
         */
        void InitializeClassifier()
        {
            m_Classifier = std::make_unique<ttool::ML::Classifier>(m_ConfigPtr->GetConfigData().ClassifierModelPath,
                                                                   m_ConfigPtr->GetConfigData().ClassifierImageSize,
                                                                   m_ConfigPtr->GetConfigData().ClassifierImageChannels,
                                                                   m_ConfigPtr->GetConfigData().ClassifierLabels,
                                                                   m_ConfigPtr->GetConfigData().ClassifierMean,
                                                                   m_ConfigPtr->GetConfigData().ClassifierStd);
        }

        /// @brief Check if the current object has changed. If yes, initialize the object tracker
        void CheckObjectChange()
        {
            if (m_ModelManagerPtr->GetObject()->getModelID() != m_CurrentObjectID)
            {
                m_CurrentObjectID = m_ModelManagerPtr->GetObject()->getModelID();
                InitializeObjectTracker();
            }
        }
    
        /**
         * @brief Initialize the camera matrix and the camera size (width and height)
         * This function should be run before initializing the view, and before initializing the object tracker
         * 
        */
        void ReadCameraMatrix()
        {
            cv::FileStorage fs(m_CameraCalibFile, cv::FileStorage::READ);
            if(!fs.isOpened()) throw std::runtime_error(std::string(__FILE__)+" could not open file: " + m_CameraCalibFile);

            int width = -1, height = -1;
            cv::Mat cameraMatrix;
            fs["camera_matrix"] >> cameraMatrix;
            fs["image_width"] >> width;
            fs["image_height"] >> height;

            if (cameraMatrix.cols == 0 || cameraMatrix.rows == 0){
                if (cameraMatrix.cols == 0 || cameraMatrix.rows == 0)
                    throw std::runtime_error(std::string(__FILE__)+" File :" + m_CameraCalibFile + " does not contains valid camera matrix");
            }

            if (width == -1 || height == 0){
                throw std::runtime_error(std::string(__FILE__)+  "File :" + m_CameraCalibFile + " does not contains valid camera dimensions");
            }

            if (cameraMatrix.type() != CV_32FC1)
                cameraMatrix.convertTo(cameraMatrix, CV_32FC1);

            CameraMatrix = cameraMatrix.clone();
            CamSize.height = height;
            CamSize.width = width;
            fs.release();
        }

        /**
         * @brief 
         * 
         * @param groundTruthPoses 
         * @return std::vector<cv::Matx44f> 
         */
        std::vector<cv::Matx44f> PreprocessGroundTruthPoses(std::vector<std::vector<float>> groundTruthPoses)
        {
            std::vector<cv::Matx44f> preprocessedGroundTruthPoses;
            for (std::vector<float> gtPose : m_ConfigPtr->GetConfigData().GroundTruthPoses)
            {
                float m00 = gtPose[0];
                float m01 = gtPose[1];
                float m02 = gtPose[2];
                float m10 = gtPose[3];
                float m11 = gtPose[4];
                float m12 = gtPose[5];
                float m20 = gtPose[6];
                float m21 = gtPose[7];
                float m22 = gtPose[8];
                float m03 = gtPose[9];
                float m13 = gtPose[10];
                float m23 = gtPose[11];

                preprocessedGroundTruthPoses.push_back(cv::Matx44f(
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                0, 0, 0, 1));
            }

            return preprocessedGroundTruthPoses;
        }

    public:
        cv::Mat CameraMatrix;
        cv::Size CamSize;
    
    private:
        std::string m_ConfigFile;
        std::shared_ptr<ttool::Config> m_ConfigPtr;
        std::string m_CameraCalibFile;

        std::shared_ptr<ttool::DModelManager> m_ModelManagerPtr;

        std::unique_ptr<ttool::ML::Classifier> m_Classifier;

        ttool::tslet::ObjectTracker m_ObjectTracker;
        ttool::InputModelManager m_Input;
        int m_CurrentObjectID = 0;

        std::unique_ptr<ttool::PoseWriter> m_PoseWriter;
    };
}

#endif // __TTOOL_H__