#include <iostream>

#include <glog/logging.h>
#include "view.hh"

using namespace std;
using namespace cv;

View *View::instance;

View::View(void)
{
	QSurfaceFormat glFormat;
	glFormat.setVersion(3, 3);
	glFormat.setProfile(QSurfaceFormat::CoreProfile);
	glFormat.setRenderableType(QSurfaceFormat::OpenGL);

	surface = new QOffscreenSurface();
	surface->setFormat(glFormat);
	surface->create();

	glContext = new QOpenGLContext();
	glContext->setFormat(surface->requestedFormat());
	glContext->create();

	silhouetteShaderProgram = new QOpenGLShaderProgram();
	phongblinnShaderProgram = new QOpenGLShaderProgram();
	normalsShaderProgram = new QOpenGLShaderProgram();

	calibrationMatrices.push_back(Matx44f::eye());

	projectionMatrix = Transformations::perspectiveMatrix(40, 4.0f / 3.0f, 0.1, 1000.0);

	lookAtMatrix = Transformations::lookAtMatrix(0, 0, 0, 0, 0, 1, 0, -1, 0);

	currentLevel = 0;
}

View::~View(void)
{
	glDeleteTextures(1, &colorTextureID);
	glDeleteTextures(1, &depthTextureID);
	glDeleteFramebuffers(1, &frameBufferID);

	delete phongblinnShaderProgram;
	delete normalsShaderProgram;
	delete silhouetteShaderProgram;
	delete surface;
}

void View::destroy()
{
	doneCurrent();

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	delete instance;
	instance = NULL;
}

void View::makeCurrent()
{
	glContext->makeCurrent(surface);
}

void View::doneCurrent()
{
	glContext->doneCurrent();
}

Matx44f View::GetCalibrationMatrix()
{
	return calibrationMatrices[currentLevel];
}

#include "shader/shaders.hh"

void View::init(const Matx33f &K, int width, int height, float zNear, float zFar, int numLevels)
{
	this->width = width;
	this->height = height;

	fullWidth = width;
	fullHeight = height;

	this->zn = zNear;
	this->zf = zFar;

	this->numLevels = numLevels;

	projectionMatrix = Transformations::perspectiveMatrix(K, width, height, zNear, zFar, true);

	makeCurrent();

	initializeOpenGLFunctions();

	// FIX FOR NEW OPENGL
	uint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	calibrationMatrices.clear();

	for (int i = 0; i < numLevels; i++)
	{
		float s = pow(2, i);

		Matx44f K_l = Matx44f::eye();
		K_l(0, 0) = K(0, 0) / s;
		K_l(1, 1) = K(1, 1) / s;
		K_l(0, 2) = K(0, 2) / s;
		K_l(1, 2) = K(1, 2) / s;

		calibrationMatrices.push_back(K_l);
	}

	// cout << "GL Version " << glGetString(GL_VERSION) << endl << "GLSL Version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	// INVERT DEPTH BUFFER
	glDepthRange(1, 0);
	glClearDepth(0.0f);
	glDepthFunc(GL_GREATER);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	initRenderingBuffers();

	shaderFolder = "src/";

	initShaderProgramFromCode(silhouetteShaderProgram, silhouette_vertex_shader, silhouette_fragment_shader);
	initShaderProgramFromCode(phongblinnShaderProgram, phongblinn_vertex_shader, phongblinn_fragment_shader);
	initShaderProgramFromCode(normalsShaderProgram, normals_vertex_shader, normals_fragment_shader);

	angle = 0;

	lightPosition = cv::Vec3f(0, 0, 0);

	// doneCurrent();
}

void View::setLevel(int level)
{
	currentLevel = level;
	int s = pow(2, currentLevel);
	width = fullWidth / s;
	height = fullHeight / s;

	width += width % 4;
	height += height % 4;
}

bool View::initRenderingBuffers()
{
	glGenTextures(1, &colorTextureID);
	glBindTexture(GL_TEXTURE_2D, colorTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &depthTextureID);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureID, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "error creating rendering buffers" << endl;
		return false;
	}
	return true;
}

bool View::initShaderProgramFromCode(QOpenGLShaderProgram *program, char *vertex_shader, char *fragment_shader)
{
	if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader))
	{
		cout << "error adding vertex shader from source file" << endl;
		return false;
	}
	if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader))
	{
		cout << "error adding fragment shader from source file" << endl;
		return false;
	}

	if (!program->link())
	{
		cout << "error linking shaders" << endl;
		return false;
	}
	return true;
}

static bool PtInFrame(const cv::Vec2f &pt, int width, int height)
{
	return (pt(0) < width && pt(1) < height && pt(0) >= 0 && pt(1) >= 0);
}

void View::RenderSilhouette(Model *model, GLenum polyonMode, bool invertDepth, float r, float g, float b, bool drawAll)
{
	vector<Model *> models;
	models.push_back(model);

	vector<Point3f> colors;
	colors.push_back(Point3f(r, g, b));

	RenderSilhouette(models, polyonMode, invertDepth, colors, drawAll);
}

void View::RenderShaded(Model *model, GLenum polyonMode, float r, float g, float b, bool drawAll)
{
	vector<Model *> models;
	models.push_back(model);

	vector<Point3f> colors;
	colors.push_back(Point3f(r, g, b));

	RenderShaded(models, polyonMode, colors, drawAll);
}

void View::RenderSilhouette(vector<Model *> models, GLenum polyonMode, bool invertDepth, const std::vector<cv::Point3f> &colors, bool drawAll)
{
	glViewport(0, 0, width, height);

	if (invertDepth)
	{
		glClearDepth(1.0f);
		glDepthFunc(GL_LESS);
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < models.size(); i++)
	{
		Model *model = models[i];

		if (model->isInitialized() || drawAll)
		{
			Matx44f pose = model->getPose();
			Matx44f normalization = model->getNormalization();

			Matx44f modelViewMatrix = lookAtMatrix * (pose * normalization);

			Matx44f modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

			silhouetteShaderProgram->bind();
			silhouetteShaderProgram->setUniformValue("uMVPMatrix", QMatrix4x4(modelViewProjectionMatrix.val));
			silhouetteShaderProgram->setUniformValue("uAlpha", 1.0f);

			Point3f color;
			if (i < colors.size())
			{
				color = colors[i];
			}
			else
			{
				color = Point3f((float)(model->getModelID()) / 255.0f, 0.0f, 0.0f);
			}
			silhouetteShaderProgram->setUniformValue("uColor", QVector3D(color.x, color.y, color.z));

			glPolygonMode(GL_FRONT_AND_BACK, polyonMode);

			model->draw(silhouetteShaderProgram);
		}
	}

	glClearDepth(0.0f);
	glDepthFunc(GL_GREATER);

	glFinish();
}

void View::ConvertMask(const cv::Mat &src_mask, cv::Mat &mask, uchar oid)
{
	mask = cv::Mat(src_mask.size(), CV_8UC1, cv::Scalar(0));
	uchar depth = src_mask.type() & CV_MAT_DEPTH_MASK;

	if (CV_8U == depth && oid > 0)
	{
		for (int r = 0; r < src_mask.rows; ++r)
			for (int c = 0; c < src_mask.cols; ++c)
			{
				if (oid == src_mask.at<uchar>(r, c))
					mask.at<uchar>(r, c) = 255;
			}
	}
	else if (CV_32F == depth)
	{
		for (int r = 0; r < src_mask.rows; ++r)
			for (int c = 0; c < src_mask.cols; ++c)
			{
				if (src_mask.at<float>(r, c))
					mask.at<uchar>(r, c) = 255;
			}
	}
	else
	{
		LOG(ERROR) << "WRONG IMAGE TYPE";
	}
}

void View::RenderShaded(vector<Model *> models, GLenum polyonMode, const std::vector<cv::Point3f> &colors, bool drawAll)
{
	glViewport(0, 0, width, height);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < models.size(); i++)
	{
		Model *model = models[i];

		if (model->isInitialized() || drawAll)
		{
			Matx44f pose = model->getPose();
			Matx44f normalization = model->getNormalization();

			Matx44f modelViewMatrix = lookAtMatrix * (pose * normalization);

			Matx33f normalMatrix = modelViewMatrix.get_minor<3, 3>(0, 0).inv().t();

			Matx44f modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

			phongblinnShaderProgram->bind();
			phongblinnShaderProgram->setUniformValue("uMVMatrix", QMatrix4x4(modelViewMatrix.val));
			phongblinnShaderProgram->setUniformValue("uMVPMatrix", QMatrix4x4(modelViewProjectionMatrix.val));
			phongblinnShaderProgram->setUniformValue("uNormalMatrix", QMatrix3x3(normalMatrix.val));
			phongblinnShaderProgram->setUniformValue("uLightPosition1", QVector3D(0.1, 0.1, -0.02));
			phongblinnShaderProgram->setUniformValue("uLightPosition2", QVector3D(-0.1, 0.1, -0.02));
			phongblinnShaderProgram->setUniformValue("uLightPosition3", QVector3D(0.0, 0.0, 0.1));
			phongblinnShaderProgram->setUniformValue("uShininess", 100.0f);
			phongblinnShaderProgram->setUniformValue("uAlpha", 1.0f);

			Point3f color;
			if (i < colors.size())
			{
				color = colors[i];
			}
			else
			{
				color = Point3f(1.0, 0.5, 0.0);
			}
			phongblinnShaderProgram->setUniformValue("uColor", QVector3D(color.x, color.y, color.z));

			glPolygonMode(GL_FRONT_AND_BACK, polyonMode);

			model->draw(phongblinnShaderProgram);
		}
	}

	glFinish();
}

Mat View::DownloadFrame(View::FrameType type)
{
	Mat res;
	switch (type)
	{
	case MASK:
		res = Mat(height, width, CV_8UC1);
		glReadPixels(0, 0, res.cols, res.rows, GL_RED, GL_UNSIGNED_BYTE, res.data);
		break;
	case RGB:
		res = Mat(height, width, CV_8UC3);
		glReadPixels(0, 0, res.cols, res.rows, GL_RGB, GL_UNSIGNED_BYTE, res.data);
		break;
	case RGB_32F:
		res = Mat(height, width, CV_32FC3);
		glReadPixels(0, 0, res.cols, res.rows, GL_RGB, GL_FLOAT, res.data);
		break;
	case DEPTH:
		res = Mat(height, width, CV_32FC1);
		glReadPixels(0, 0, res.cols, res.rows, GL_DEPTH_COMPONENT, GL_FLOAT, res.data);
		break;
	default:
		res = Mat::zeros(height, width, CV_8UC1);
		break;
	}
	return res;
}