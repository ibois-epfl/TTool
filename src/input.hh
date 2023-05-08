#pragma once

#define KEY_UP 82

#include <fstream>
#include <iomanip>

#include "model.hh"
#include "transformations.hh"
#include "d_model_manager.hh"

namespace ttool
{
    struct Input
    {
        public:
        Input(std::shared_ptr<DModelManager> modelManagerPtr)
        {
            m_ModelManagerPtr = modelManagerPtr;
            m_PoseOutput = "input_pose.txt";
        }

        /**
         * @brief Set the pose output file
         * 
         * @param poseOutput 
         */
        void SetPoseOutput(std::string poseOutput)
        {
            m_PoseOutput = poseOutput;
        }

        void SetGtPoseOutput(std::string gtPoseOutput)
        {
            m_GtPoseOutput = gtPoseOutput;
        }

        /**
         * @brief Get the pose
         * 
         */
        cv::Matx44f GetPose()
        {
            return m_ModelManagerPtr->GetObject()->getPose();
        }

        /**
         * @brief Consume a key press and perform the corresponding action
         * 
         * @param key translate (w, s, a, d, q, e) or rotate (i, k, j, l, u, o
         */
        void ConsumeKey(char key)
        {
            // if (-1 != int(key))
            //     std::cout << "Key: " << std::isprint(key) << " " << int(key) << std::endl;
            switch (key)
            {
            // Change the model
            case char(KEY_UP):
                m_ModelManagerPtr->IncreaseObjectID();
                break;
            case 'r':
                m_ModelManagerPtr->ResetObjectToGroundTruth();
                break;
            // Translate the model
            case 'w':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, -1.0f, 0.0f));
                break;
            case 's':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, 1.0f, 0.0f));
                break;
            case 'a':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(-1.0f, 0.0f, 0.0f));
                break;
            case 'd':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(1.0f, 0.0f, 0.0f));
                break;
            case 'q':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, 0.0f, -1.0f));
                break;
            case 'e':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, 0.0f, 1.0f));
                break;
            // Rotate the model
            case 'i':
                Rotate(m_ModelManagerPtr->GetObject(), 1.0f, cv::Vec3f(1.0f, 0.0f, 0.0f));
                break;
            case 'k':
                Rotate(m_ModelManagerPtr->GetObject(), 1.0f, cv::Vec3f(-1.0f, 0.0f, 0.0f));
                break;
            case 'j':
                Rotate(m_ModelManagerPtr->GetObject(), 1.0f, cv::Vec3f(0.0f, 0.0f, 1.0f));
                break;
            case 'l':
                Rotate(m_ModelManagerPtr->GetObject(), 1.0f, cv::Vec3f(0.0f, 0.0f, -1.0f));
                break;
            case 'u':
                Rotate(m_ModelManagerPtr->GetObject(), 1.0f, cv::Vec3f(0.0f, 1.0f, 0.0f));
                break;
            case 'o':
                Rotate(m_ModelManagerPtr->GetObject(), 1.0f, cv::Vec3f(0.0f, -1.0f, 0.0f));
                break;
            case 'y':
            {
                cv::Matx44f pose = m_ModelManagerPtr->GetObject()->getPose();
                std::cout << "pose: " << pose << std::endl;
                m_ModelManagerPtr->GetObject()->setInitialPose(pose);

                std::ifstream fsRead(m_GtPoseOutput);
                std::vector<std::string> lines;
                std::string line;
                if (fsRead.is_open())
                {
                    while (getline(fsRead, line))
                    {
                        lines.push_back(line);
                    }
                }
                fsRead.close();

                int lineIndex = m_ModelManagerPtr->GetObject()->getModelID() - 1;
                lines[lineIndex + 2] = 
                      std::to_string(pose(0, 0)) + " " + std::to_string(pose(0, 1)) + " " + std::to_string(pose(0, 2)) + " " + std::to_string(pose(1, 0)) + " "
                    + std::to_string(pose(1, 1)) + " " + std::to_string(pose(1, 2)) + " " + std::to_string(pose(2, 0)) + " " + std::to_string(pose(2, 1)) + " "
                    + std::to_string(pose(2, 2)) + " " + std::to_string(pose(0, 3)) + " " + std::to_string(pose(1, 3)) + " " + std::to_string(pose(2, 3));
                
                std::ofstream fsWrite;
                fsWrite.open(m_GtPoseOutput);
                if (!fsWrite.is_open())
                {
                    std::cerr << "failed to open pose file, press any key to exit" << std::endl;
                    getchar();
                    exit(-1);
                }
                for (auto &line : lines)
                {
                    fsWrite << line << std::endl;
                }
                fsWrite.close();
                break;
            }
            // Save the pose of the model
            case 'p':
            {
                cv::Matx44f pose = m_ModelManagerPtr->GetObject()->getPose();
                std::cout << "pose: " << pose << std::endl;
                m_ModelManagerPtr->GetObject()->setInitialPose(pose);
                std::ofstream fs;
                fs.open(m_PoseOutput, std::ios_base::app);
                if (!fs.is_open()) {
                    std::cerr << "failed to open pose file, press any key to exit" << std::endl;
                    getchar();
                    exit(-1);
                }

                std::time_t t = std::time(nullptr);
                std::tm tm = *std::localtime(&t);
                fs << "# " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;
                fs << pose(0, 0) << " " << pose(0, 1) << " " << pose(0, 2) << " " << pose(1, 0) << " "
                    << pose(1, 1) << " " << pose(1, 2) << " " << pose(2, 0) << " " << pose(2, 1) << " "
                    << pose(2, 2) << " " << pose(0, 3) << " " << pose(1, 3) << " " << pose(2, 3) << std::endl;
                fs.close();
                break;
            }
            default:
                break;
            }
        }

        private:
        /**
         * @brief Translate a model
         * 
         * @param model shared pointer to the model
         * @param translation world coordinate translation vector - think of this as a global translation (i.e. when the model is translated, the translation will not change)
         */
        void Translate(std::shared_ptr<Model> model, cv::Vec3f translation)
        {
            cv::Matx44f pose = model->getPose();
            cv::Matx44f translate = Transformations::translationMatrix(translation);
            pose = translate * pose;
            model->setPose(pose);
        }

        /**
         * @brief Rotate a model around a given axis
         * 
         * The rotation is done around the center of the model
         * Note: LBN and RTF are AABB coordinates of the model
         * 
         * @param model shared pointer to the model
         * @param axis world axis of rotation - think of this as a global axis (i.e. when the model is rotated, the axis will not change)
         */
        void Rotate(std::shared_ptr<Model> model, float angle, cv::Vec3f axis)
        {
            cv::Vec3f center = (model->getLBN() + model->getRTF()) / 2.0f;

            cv::Matx44f pose = model->getPose();
            cv::Matx44f normalization = model->getNormalization();

            cv::Vec4f localAxis = (pose * normalization) * cv::Vec4f(axis(0), axis(1), axis(2), 0.0f);

            cv::Vec4f worldCenter = (pose * normalization) * cv::Vec4f(center(0), center(1), center(2), 1.0f);
            worldCenter = worldCenter / worldCenter(3);
            pose = Transformations::translationMatrix(-worldCenter(0), -worldCenter(1), -worldCenter(2)) * pose;
            // pose = Transformations::rotationMatrix(angle, axis) * pose;
            pose = Transformations::rotationMatrix(angle, cv::Vec3f(localAxis(0), localAxis(1), localAxis(2))) * pose;
            pose = Transformations::translationMatrix(worldCenter(0), worldCenter(1), worldCenter(2)) * pose;
            model->setPose(pose);
        }

        private:
        std::string m_PoseOutput;
        std::string m_GtPoseOutput;
        std::shared_ptr<DModelManager> m_ModelManagerPtr;
    };
    

}