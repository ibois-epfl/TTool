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

	static View *Instance(void)
	{
		if (instance == NULL)
			instance = new View();
		return instance;
	}

	bool IsInFrame(cv::Point2f &pt)
	{
		return (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height);
	}

	void init(const cv::Matx33f &K, int width, int height, float zNear, float zFar, int numLevels);

	void setLevel(int level);
	int getLevel();

	float getZNear();
	float getZFar();

	int GetWidth() { return fullWidth; }
	int GetHeight() { return fullHeight; }
	cv::Matx44f GetCalibrationMatrix();

	void RenderSilhouette(std::shared_ptr<Model> model, GLenum polyonMode, bool invertDepth = false, const std::vector<cv::Point3f> &colors = std::vector<cv::Point3f>(), bool drawAll = false);
	void RenderShaded(std::shared_ptr<Model> model, GLenum polyonMode, const cv::Point3f color = cv::Point3f(1.0, 0.5, 0.0), bool drawAll = false);

	void ConvertMask(const cv::Mat &src_mask, cv::Mat &mask, uchar oid);

	void ProjectBoundingBox(std::shared_ptr<Model> model, std::vector<cv::Point2f>& projections, cv::Rect& boundingRect);

	cv::Mat DownloadFrame(View::FrameType type);

	void destroy();
	
private:
	static View *instance;
	
	bool m_IsInitialized = false;

	int width;
	int height;

	int fullWidth;
	int fullHeight;

	float zn;
	float zf;

	int numLevels;

	int currentLevel;

	std::vector<cv::Matx44f> calibrationMatrices;
	cv::Matx44f projectionMatrix;
	cv::Matx44f lookAtMatrix;

	GLuint frameBufferID;
	GLuint colorTextureID;
	GLuint depthTextureID;

	int angle;

	cv::Vec3f lightPosition;

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

	GLuint m_SilhouetteShaderProgram;
	GLuint m_SilhouetteMatrixId;
	GLuint m_SilhouetteAlphaId;
	GLuint m_SilhouetteColorId;
	GLuint m_VAO;

	bool initRenderingBuffers();
};
