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

namespace ttool
{
    class TTOOL_API TTool
    {
    public:
        TTool(std::string configFile, std::string cameraCalibFile)
        {
            std::cout << "ttool constructor cmd calib file: " << cameraCalibFile << std::endl;
            m_ConfigPtr = std::make_shared<ttool::Config>(configFile);
            m_CameraCalibFile = cameraCalibFile;
            
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
        };

        TTool(std::string configFile, cv::Mat cameraMatrix, cv::Size cameraSize)
        {
            m_ConfigPtr = std::make_shared<ttool::Config>(configFile);
            

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
         * @brief Run the object tracker on a frame
         * 
         * @param frame 
         */
        void RunOnAFrame(cv::Mat frame)
        {
            CheckObjectChange();
            m_ObjectTracker.UpdateHistogram(m_CurrentObjectID, frame);
            m_ObjectTracker.CallEstimatePose(m_CurrentObjectID, frame);
        }

        /**
         * @brief DEBUGGING FUNCTION - Show the silhouette of the model on the camera frame
         * 
         * @param frame camera frame  
         */
        void ShowSilhouette(cv::Mat frame, int waitTime = -1)
        {
            cv::Mat result = frame.clone();
            
            View *view = View::Instance();
            view->RenderSilhouette(m_ModelManagerPtr->GetObject(), GL_FILL);
            cv::Mat depth = view->DownloadFrame(View::DEPTH); // This is the depth map of the model 0.0f and 1.0f are the min and max depth

            float alpha = 0.5f;
            for (int y = 0; y < frame.rows; y++)
            	for (int x = 0; x < frame.cols; x++)
            	{
                    cv::Vec3b color = cv::Vec3b(255, 128, 0);
            		if (depth.at<float>(y, x) != 0.0f)
            		{
                        result.at<cv::Vec3b>(y, x) = alpha * color + (1.0f - alpha) * result.at<cv::Vec3b>(y, x);
            		}
            	}

            cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
            cv::imshow("TTool Debugging Window", result);
            if (waitTime >= 0)
                cv::waitKey(waitTime);
        }

        void DestrolView()
        {
            View *view = View::Instance();
            view->destroy();
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
        std::shared_ptr<Object3D> GetModel()
        {
            CheckObjectChange();
            return m_ModelManagerPtr->GetObject();
        }


        /**
         * @brief Set the Object ID
         * 
         * @param objectID 
         */
        void SetObjectID(int objectID)
        {
            m_CurrentObjectID = objectID;
            m_ModelManagerPtr->SetObjectID(objectID);
            InitializeObjectTracker();
        }

        void InitializeObjectTracker()
        {
            m_ObjectTracker.Consume(m_ModelManagerPtr->GetObject()->getModelID(), m_ModelManagerPtr->GetObject(), CameraMatrix);
        }

        void InitializeView()
        {
            // Initialize the view
            View* view = View::Instance();
            float zn = m_ConfigPtr->GetConfigData().Zn;
            float zf = m_ConfigPtr->GetConfigData().Zf;
            view->init(CameraMatrix, CamSize.width, CamSize.height, zn, zf, 4);
        }


    public:
        std::shared_ptr<ttool::Config> GetConfig() { return m_ConfigPtr; };
        std::shared_ptr<ttool::DModelManager> GetModelManager() { return m_ModelManagerPtr; };
        int GetCurrentObjectID() { return m_CurrentObjectID; };

    private:
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
        std::shared_ptr<ttool::Config> m_ConfigPtr;
        std::string m_CameraCalibFile;

        std::shared_ptr<ttool::DModelManager> m_ModelManagerPtr;

        tslet::ObjectTracker m_ObjectTracker;
        ttool::InputModelManager m_Input;
        int m_CurrentObjectID = 0;
    };
}

#endif // __TTOOL_H__