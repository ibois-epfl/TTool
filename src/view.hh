#pragma once

#include <iostream>
#include <vector>

#include <QOpenGLContext>
#include <QOffscreenSurface>

#include <QGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "transformations.hh"
#include "model.hh"

class View : public QOpenGLFunctions_3_3_Core
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

	int GetWidth() { return fullWidth; }
	int GetHeight() { return fullHeight; }
	cv::Matx44f GetCalibrationMatrix();

	void RenderSilhouette(Model *model, GLenum polyonMode, bool invertDepth = false, float r = 1.0f, float g = 1.0f, float b = 1.0f, bool drawAll = false);
	void RenderSilhouette(std::vector<Model *> models, GLenum polyonMode, bool invertDepth = false, const std::vector<cv::Point3f> &colors = std::vector<cv::Point3f>(), bool drawAll = false);

	void RenderShaded(Model *model, GLenum polyonMode, float r = 1.0f, float g = 0.5f, float b = 0.0f, bool drawAll = false);
	void RenderShaded(std::vector<Model *> models, GLenum polyonMode, const std::vector<cv::Point3f> &colors = std::vector<cv::Point3f>(), bool drawAll = false);

	void ConvertMask(const cv::Mat &src_mask, cv::Mat &mask, uchar oid);

	cv::Mat DownloadFrame(View::FrameType type);

	void destroy();

protected:
	void makeCurrent();
	void doneCurrent();

private:
	static View *instance;

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

	QOffscreenSurface *surface;
	QOpenGLContext *glContext;

	GLuint frameBufferID;
	GLuint colorTextureID;
	GLuint depthTextureID;

	int angle;

	cv::Vec3f lightPosition;

	QString shaderFolder;
	QOpenGLShaderProgram *silhouetteShaderProgram;
	QOpenGLShaderProgram *phongblinnShaderProgram;
	QOpenGLShaderProgram *normalsShaderProgram;

	bool initRenderingBuffers();
	bool initShaderProgramFromCode(QOpenGLShaderProgram *program, char *vertex_shader, char *fragment_shader);
};
