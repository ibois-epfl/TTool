/**
 * This file has been modified by Andrea Settimi, Naravich Chutisilp (IBOIS, EPFL) 
 * from SLET with Copyright (C) 2020  Hong Huang and Fan Zhong and Yuqing Sun and Xueying Qin (Shandong University)
 *                     
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

#include <iostream>
#include <vector>

#include <GL/glew.h>

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "transformations.hh"
#include "model.hh"

namespace ttool
{
	/**
	 * @brief This class is used to render the model and get the depth map
	 * It is a singleton class and can be accessed by View::Instance()
	 * 
	 */
	class View
	{
	public:
		enum FrameType
		{
			MASK,
			RGB,
			RGB_32F,
			DEPTH
		};

		View(void);

		~View(void);

		/**
		 * @brief Get the singleton instance of the View class
		 * 
		 * @return View* 
		 */
		static View *Instance(void)
		{
			if (m_Instance == NULL)
				m_Instance = new View();
			return m_Instance;
		}

		/**
		 * @brief Initialize the view
		 * 
		 * @param K 		 3x3 Camera matrix
		 * @param width 	 Width of the view
		 * @param height     Height of the view
		 * @param zNear      Near clipping plane
		 * @param zFar 	     Far clipping plane
		 * @param numLevels  Number of levels of the view
		 */
		void Initialize(const cv::Matx33f &K, int width, int height, float zNear, float zFar, int numLevels);

	public: // Getters and setters
		void SetLevel(int level);
		int GetLevel();

		float GetZNear() { return m_Zn; };
		float GetZFar() { return m_Zf; }

		int GetWidth() { return m_FullWidth; }
		int GetHeight() { return m_FullHeight; }

		cv::Matx44f GetCalibrationMatrix();

	public:
		/**
		 * @brief Render depth map of the model
		 * 
		 * @param model 		Model to be rendered
		 * @param polygonMode 	Polygon rasterization mode
		 * @param invertDepth   Whether to invert the depth map (so the model is black and the background is white)
		 */
		void RenderSilhouette(std::shared_ptr<ttool::tslet::Model> model, GLenum polygonMode, bool invertDepth = false);

		/**
		 * @brief Render phongblinn shaded model
		 * 
		 * @param model     	Model to be rendered
		 * @param polygonMode 	Polygon rasterization mode
		 * @param color 		The color of the model
		 */
		void RenderShaded(std::shared_ptr<ttool::tslet::Model> model, GLenum polygonMode, const cv::Point3f color = cv::Point3f(1.0, 0.5, 0.0));

		/**
		 * @brief Convert a mask to 0 and 255 values
		 * If the mask is a 32F image, every non-zero value is converted to 255
		 * If the mask is a depth map (representing multiple model instances by its id), the pixel with the given id is set to 255, all other pixels are set to 0
		 * 
		 * @param src_mask  Input mask
		 * @param mask 		Output mask
		 * @param oid      	Object id of the model
		 */
		void ConvertMask(const cv::Mat &src_mask, cv::Mat &mask, uchar oid);

		/**
		 * @brief Project the bounding box of the model to the view
		 * 
		 * @param model 	   Model to be projected
		 * @param projections  Resulting projections of the bounding box
		 * @param boundingRect Resulting bounding box
		 */
		void ProjectBoundingBox(std::shared_ptr<ttool::tslet::Model> model, std::vector<cv::Point2f>& projections, cv::Rect& boundingRect);

		/**
		 * @brief Download a frame from the view
		 * 
		 * @param type 		   Type of the frame
		 * @return cv::Mat 
		 */
		cv::Mat DownloadFrame(View::FrameType type);

		void Destroy();

	private:
		/**
		 * @brief Initialize the rendering buffers
		 * 
		 * @return true 
		 * @return false 
		 */
		bool InitRenderingBuffers();

	private:
		static View *m_Instance;
		
		bool m_IsInitialized = false;

		int m_Width;
		int m_Height;

		int m_FullWidth;
		int m_FullHeight;

		float m_Zn;
		float m_Zf;

		int m_NumLevels;

		int m_CurrentLevel;

		std::vector<cv::Matx44f> m_CalibrationMatrices;
		cv::Matx44f m_ProjectionMatrix;
		cv::Matx44f m_LookAtMatrix;

		GLuint m_FrameBufferID;
		GLuint m_ColorTextureID;
		GLuint m_DepthTextureID;

		// Phongblinn shader
		GLuint m_PhongblinnShaderProgram;
		GLuint m_PhongblinnMVPMatrixID;
		GLuint m_PhongblinnMVMatrixID;
		GLuint m_PhongblinnNormalMatrixID;
		GLuint m_PhongblinnLightPosition1ID;
		GLuint m_PhongblinnLightPosition2ID;
		GLuint m_PhongblinnLightPosition3ID;
		GLuint m_PhongblinnShininessID;
		GLuint m_PhongblinnAlphaID;
		GLuint m_PhongblinnColorID;

		// Silhouette shader
		GLuint m_SilhouetteShaderProgram;
		GLuint m_SilhouetteMatrixId;
		GLuint m_SilhouetteAlphaId;
		GLuint m_SilhouetteColorId;
		GLuint m_VAO;

	};
}