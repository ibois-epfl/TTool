#pragma once

#include "model.hh"
#include "transformations.hh"

namespace ttool
{
    struct Input
    {
        public:
        Input(std::shared_ptr<Model> model)
        {
            m_Model = model;
        }

        /**
         * @brief Consume a key press and perform the corresponding action
         * 
         * @param key translate (w, s, a, d, q, e) or rotate (i, k, j, l, u, o
         */
        void ConsumeKey(char key)
        {
            switch (key)
            {
            // Translate the model
            case 'w':
                Translate(m_Model, cv::Vec3f(0.0f, -1.0f, 0.0f));
                break;
            case 's':
                Translate(m_Model, cv::Vec3f(0.0f, 1.0f, 0.0f));
                break;
            case 'a':
                Translate(m_Model, cv::Vec3f(-1.0f, 0.0f, 0.0f));
                break;
            case 'd':
                Translate(m_Model, cv::Vec3f(1.0f, 0.0f, 0.0f));
                break;
            case 'q':
                Translate(m_Model, cv::Vec3f(0.0f, 0.0f, -1.0f));
                break;
            case 'e':
                Translate(m_Model, cv::Vec3f(0.0f, 0.0f, 1.0f));
                break;
            // Rotate the model
            case 'i':
                Rotate(m_Model, 1.0f, cv::Vec3f(1.0f, 0.0f, 0.0f));
                break;
            case 'k':
                Rotate(m_Model, 1.0f, cv::Vec3f(-1.0f, 0.0f, 0.0f));
                break;
            case 'j':
                Rotate(m_Model, 1.0f, cv::Vec3f(0.0f, 0.0f, 1.0f));
                break;
            case 'l':
                Rotate(m_Model, 1.0f, cv::Vec3f(0.0f, 0.0f, -1.0f));
                break;
            case 'u':
                Rotate(m_Model, 1.0f, cv::Vec3f(0.0f, 1.0f, 0.0f));
                break;
            case 'o':
                Rotate(m_Model, 1.0f, cv::Vec3f(0.0f, -1.0f, 0.0f));
                break;
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

            auto worldCenter = (pose * normalization) * cv::Vec4f(center(0), center(1), center(2), 1.0f);
            worldCenter = worldCenter / worldCenter(3);
            pose = Transformations::translationMatrix(-worldCenter(0), -worldCenter(1), -worldCenter(2)) * pose;
            pose = Transformations::rotationMatrix(angle, axis) * pose;
            pose = Transformations::translationMatrix(worldCenter(0), worldCenter(1), worldCenter(2)) * pose;
            model->setPose(pose);
        }

        private:
        std::shared_ptr<Model> m_Model;
    };
    

}