#pragma once

#include "model.hh"
#include "transformations.hh"

namespace ttool
{
    struct Input
    {
        public:
        void Translate(cv::Matx44f &pose, std::shared_ptr<Model> modelPtr)
        {
            cv::Matx44f translation = Transformations::translationMatrix(0.0f, 0.0f, 3.0f);
            pose = translation * pose;
            cv::Vec3f center = (modelPtr->getLBN() + modelPtr->getRTF()) / 2.0f;
            std::cout << "Center: " << center << std::endl;
        }
        void Rotate2(std::shared_ptr<Model> modelPtr)
        {
            // cv::Matx44f normalization = modelPtr->getNormalization();
            cv::Vec3f center = (modelPtr->getLBN() + modelPtr->getRTF()) / 2.0f;
            cv::Vec3f lbn = modelPtr->getLBN();
            cv::Vec3f rtf = modelPtr->getRTF();

            // calculate other 6 coordinates
            cv::Vec3f ltn(lbn(0), rtf(1), lbn(2));
            cv::Vec3f rbn(lbn(0), lbn(1), rtf(2));
            cv::Vec3f rtn(rtf(0), rtf(1), lbn(2));
            cv::Vec3f ltf(lbn(0), rtf(1), rtf(2));
            cv::Vec3f lbf(lbn(0), lbn(1), rtf(2));
            cv::Vec3f rbf(rtf(0), lbn(1), rtf(2));
            cv::Vec3f axis;
            axis = lbf - lbn;
            axis = ltn - lbn;
            axis = rbn - lbn;

            axis = cv::Vec3f(0.0f, 1.0f, 0.0f);
            cv::Matx44f rotate = Transformations::rotationMatrix(10.0f, axis);

            cv::Matx44f pose = modelPtr->getPose();
            auto worldCenter = pose * cv::Vec4f(center(0), center(1), center(2), 1.0f);
            worldCenter = worldCenter / worldCenter(3);
            pose = Transformations::translationMatrix(0, 0, -worldCenter(2)) * pose;
            pose = rotate * pose;
            pose = Transformations::translationMatrix(0, 0, worldCenter(2)) * pose;
            modelPtr->setPose(pose);

            // cv::Vec3f center_world;
            // cv::Matx41f center_homog(center(0), center(1), center(2), 1.0f);
            // cv::Matx41f center_world_homog = (pose * normalization) * center_homog;
            // center_world = cv::Vec3f(center_world_homog(0), center_world_homog(1), center_world_homog(2));
            // std::cout << "Center world: " << center_world << std::endl;
            // cv::Matx44f translate2Origin = Transformations::translationMatrix(-center_world);
            // cv::Matx44f translate2Original = Transformations::translationMatrix(center_world);
            // cv::Matx44f rotate = Transformations::rotationMatrix(2.0f, cv::Vec3f(0.0f, 1.0f, 0.0f));
            // pose = translate2Origin * rotate * translate2Original * pose;
            // std::cout << "Pose: \n" << pose << std::endl;

        }

        void Rotate(std::shared_ptr<Model> model)
        {
            // cv::Matx44f normalization = modelPtr->getNormalization();
            cv::Vec3f center = (model->getLBN() + model->getRTF()) / 2.0f;
            cv::Vec3f lbn = model->getLBN();
            cv::Vec3f rtf = model->getRTF();

            // calculate other 6 coordinates
            cv::Vec3f ltn(lbn(0), rtf(1), lbn(2));
            cv::Vec3f rbn(lbn(0), lbn(1), rtf(2));
            cv::Vec3f rtn(rtf(0), rtf(1), lbn(2));
            cv::Vec3f ltf(lbn(0), rtf(1), rtf(2));
            cv::Vec3f lbf(lbn(0), lbn(1), rtf(2));
            cv::Vec3f rbf(rtf(0), lbn(1), rtf(2));
            cv::Vec3f axis;
            // axis = lbf - lbn;
            // axis = ltn - lbn;
            // axis = rbn - lbn;

            axis = cv::Vec3f(1.0f, 0.0f, 0.0f);
            std::cout << "Axis: " << axis << std::endl;
            cv::Matx44f rotate = Transformations::rotationMatrix(10.0f, axis);

            cv::Matx44f pose = model->getPose();
            cv::Matx44f normalization = model->getNormalization();

            auto worldCenter = (pose * normalization) * cv::Vec4f(center(0), center(1), center(2), 1.0f);
            // auto worldCenter = cv::Vec4f(center(0), center(1), center(2), 1.0f);
            worldCenter = worldCenter / worldCenter(3);
            // std::cout << "Press any key to continue...\n";
            // cv::waitKey(0);
            // std::cout << "To World center: " << worldCenter << std::endl;
            pose = Transformations::translationMatrix(-worldCenter(0), -worldCenter(1), -worldCenter(2)) * pose;
            // model->setPose(pose);
            // cv::waitKey(0);
            // std::cout << "Rotate" << std::endl;
            pose = rotate * pose;
            // model->setPose(pose);
            // cv::waitKey(0);
            // std::cout << "Back World center: " << worldCenter << std::endl;
            pose = Transformations::translationMatrix(worldCenter(0), worldCenter(1), worldCenter(2)) * pose;
            model->setPose(pose);

            // cv::Vec3f center_world;
            // cv::Matx41f center_homog(center(0), center(1), center(2), 1.0f);
            // cv::Matx41f center_world_homog = (pose * normalization) * center_homog;
            // center_world = cv::Vec3f(center_world_homog(0), center_world_homog(1), center_world_homog(2));
            // std::cout << "Center world: " << center_world << std::endl;
            // cv::Matx44f translate2Origin = Transformations::translationMatrix(-center_world);
            // cv::Matx44f translate2Original = Transformations::translationMatrix(center_world);
            // cv::Matx44f rotate = Transformations::rotationMatrix(2.0f, cv::Vec3f(0.0f, 1.0f, 0.0f));
            // pose = translate2Origin * rotate * translate2Original * pose;
            // std::cout << "Pose: \n" << pose << std::endl;

        }


    };
    

}