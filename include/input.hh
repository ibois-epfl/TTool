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

#pragma once

#define KEY_UP 82

#include <fstream>
#include <iomanip>

#include "model.hh"
#include "transformations.hh"
#include "d_model_manager.hh"
#include "visualizer.hh"

namespace ttool
{
    /**
     * @brief This class implements input handling for the model management.
     * Hence, it is responsible for translating and rotating the model.
     * Additionally, it is responsible for changing the model, saving the pose to the config file, and resetting the pose to the ground truth pose and more.
     */
    class InputModelManager
    {
    public:
        InputModelManager(){};

        InputModelManager(std::shared_ptr<DModelManager> modelManagerPtr)
        {
            m_ModelManagerPtr = modelManagerPtr;
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
         * @brief Consume a key press and perform the corresponding action related to the model manager
         * 
         * @param key translate (w, s, a, d, q, e) or rotate (i, k, j, l, u, o
         */
        void ConsumeKey(char key)
        {
            switch (key)
            {
            // Adjust the translation and rotation scale
            case '1':
                translateScale = translateScale / 2.0f;
                rotateScale = rotateScale / 2.0f;
                break;
            case '2':
                translateScale = translateScale * 2.0f;
                rotateScale = rotateScale * 2.0f;
                break;

            // Change the model
            case char(KEY_UP):
                m_ModelManagerPtr->IncreaseObjectID();
                break;
            
            // Resetting the model
            case 'r':
                m_ModelManagerPtr->ResetObjectToConfigInitialPose();
                break;
            case 't':
                m_ModelManagerPtr->ResetObjectToLastSavePose();
                break;
                

            // Translate the model
            case 'w':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, -translateScale, 0.0f));
                break;
            case 's':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, translateScale, 0.0f));
                break;
            case 'a':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(-translateScale, 0.0f, 0.0f));
                break;
            case 'd':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(translateScale, 0.0f, 0.0f));
                break;
            case 'q':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, 0.0f, -translateScale));
                break;
            case 'e':
                Translate(m_ModelManagerPtr->GetObject(), cv::Vec3f(0.0f, 0.0f, translateScale));
                break;

            // Rotate the model
            case 'i':
                Rotate(m_ModelManagerPtr->GetObject(), rotateScale, cv::Vec3f(1.0f, 0.0f, 0.0f));
                break;
            case 'k':
                Rotate(m_ModelManagerPtr->GetObject(), rotateScale, cv::Vec3f(-1.0f, 0.0f, 0.0f));
                break;
            case 'j':
                Rotate(m_ModelManagerPtr->GetObject(), rotateScale, cv::Vec3f(0.0f, 0.0f, 1.0f));
                break;
            case 'l':
                Rotate(m_ModelManagerPtr->GetObject(), rotateScale, cv::Vec3f(0.0f, 0.0f, -1.0f));
                break;
            case 'u':
                Rotate(m_ModelManagerPtr->GetObject(), rotateScale, cv::Vec3f(0.0f, 1.0f, 0.0f));
                break;
            case 'o':
                Rotate(m_ModelManagerPtr->GetObject(), rotateScale, cv::Vec3f(0.0f, -1.0f, 0.0f));
                break;
            
            // Save the pose of the model
            case 'y':
            {
                cv::Matx44f pose = m_ModelManagerPtr->GetObject()->getPose();
                std::cout << "pose: " << pose << std::endl;
                m_ModelManagerPtr->GetObject()->setInitialPose(pose);
                m_ModelManagerPtr->SaveObjectInitialPose(pose);
                m_ModelManagerPtr->SavePosesToConfig();
                break;
            }
            case 'p':
            {
                cv::Matx44f pose = m_ModelManagerPtr->GetObject()->getPose();
                std::cout << "pose: " << pose << std::endl;
                m_ModelManagerPtr->SaveObjectInitialPose(pose);
                m_ModelManagerPtr->GetObject()->setInitialPose(pose);
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
        void Translate(std::shared_ptr<ttool::tslet::Model> model, cv::Vec3f translation)
        {
            cv::Matx44f pose = model->getPose();
            cv::Matx44f translate = ttool::utils::Transformations::translationMatrix(translation);
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
        void Rotate(std::shared_ptr<ttool::tslet::Model> model, float angle, cv::Vec3f axis)
        {
            cv::Vec3f center = (model->getLBN() + model->getRTF()) / 2.0f;

            cv::Matx44f pose = model->getPose();
            cv::Matx44f normalization = model->getNormalization();

            cv::Vec4f localAxis = (pose * normalization) * cv::Vec4f(axis(0), axis(1), axis(2), 0.0f);

            cv::Vec4f worldCenter = (pose * normalization) * cv::Vec4f(center(0), center(1), center(2), 1.0f);
            worldCenter = worldCenter / worldCenter(3);
            pose = ttool::utils::Transformations::translationMatrix(-worldCenter(0), -worldCenter(1), -worldCenter(2)) * pose;
            pose = ttool::utils::Transformations::rotationMatrix(angle, cv::Vec3f(localAxis(0), localAxis(1), localAxis(2))) * pose;
            pose = ttool::utils::Transformations::translationMatrix(worldCenter(0), worldCenter(1), worldCenter(2)) * pose;
            model->setPose(pose);
        }

        std::shared_ptr<DModelManager> m_ModelManagerPtr;
        float translateScale = 0.01f;
        float rotateScale = 0.5f;
    };

    /**
     * @brief This class implements input handling for the the visualizer.
     * Hence, it is responsible for showing help menu.
     */
    class InputVisualizer
    {
    public:
        InputVisualizer() {}

        InputVisualizer(std::shared_ptr<ttool::standaloneUtils::Visualizer> visualizerPtr)
        {
            m_VisualizerPtr = visualizerPtr;
        }

        /**
         * @brief Consume a key press and perform the corresponding action related to the visualizer
         * 
         * @param key the key that was pressed
         */
        void ConsumeKey(char key)
        {
            switch (key)
            {
            // Visualizer options
            case 'h':
            {
                m_VisualizerPtr->ToggleShowKeymaps();
                break;
            }
            default:
                break;
            }
        }

    private:
        std::shared_ptr<ttool::standaloneUtils::Visualizer> m_VisualizerPtr;
    };

    /**
     * @brief This class implements input handling for the both the model manager and the visualizer.
     * It is a wrapper class for the InputModelManager and InputVisualizer classes.
     * 
     */
    class Input
    {
    public:
        Input(std::shared_ptr<DModelManager> modelManagerPtr, std::shared_ptr<ttool::standaloneUtils::Visualizer> visualizerPtr)
        {
            m_InputModelManager = InputModelManager(modelManagerPtr);
            m_InputVisualizer = InputVisualizer(visualizerPtr);
        }

        /**
         * @brief Get the pose
         * 
         */
        cv::Matx44f GetPose()
        {
            return m_InputModelManager.GetPose();
        }

        /**
         * @brief Consume a key press and perform the corresponding action related to the model manager
         * 
         * @param key translate (w, s, a, d, q, e) or rotate (i, k, j, l, u, o)
         */
        void ConsumeKey(char key)
        {
            m_InputModelManager.ConsumeKey(key);
            m_InputVisualizer.ConsumeKey(key);
        }

    private:
        InputModelManager m_InputModelManager;
        InputVisualizer m_InputVisualizer;
    };
}
