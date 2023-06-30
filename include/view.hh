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
	void RenderSilhouette(std::vector<std::shared_ptr<Model>> models, GLenum polyonMode, bool invertDepth = false, const std::vector<cv::Point3f> &colors = std::vector<cv::Point3f>(), bool drawAll = false);

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

	GLuint silhouetteShaderProgram;
	GLuint m_MatrixId;
	GLuint m_AlphaId;
	GLuint m_ColorId;
	GLuint m_VAO;

	bool initRenderingBuffers();
};
