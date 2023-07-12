#include <iostream>

#include <glog/logging.h>
#include "view.hh"
#include "shader.hh"
#include "util.hh"

using namespace std;
using namespace cv;

View *View::instance;

View::View(void)
{
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

	// delete phongblinnShaderProgram;
	// delete normalsShaderProgram;
	// delete m_SilhouetteShaderProgram;
	// delete surface;
}

void View::destroy()
{
	// doneCurrent();
	projectionMatrix = Matx44f::eye();
	lookAtMatrix = Matx44f::eye();
	calibrationMatrices.clear();
	m_IsInitialized = false;

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	delete instance;
	instance = NULL;
}

Matx44f View::GetCalibrationMatrix()
{
	return calibrationMatrices[currentLevel];
}

void View::init(const Matx33f &K, int width, int height, float zNear, float zFar, int numLevels)
{
	if (m_IsInitialized)
	{
		LOG(WARNING) << "View already initialized";
		return;
	}

	this->width = width;
	this->height = height;

	fullWidth = width;
	fullHeight = height;

	this->zn = zNear;
	this->zf = zFar;

	this->numLevels = numLevels;

	projectionMatrix = Transformations::perspectiveMatrix(K, width, height, this->zn, this->zf, true);

	LOG(INFO) << "Projection matrix: " << endl
			  << projectionMatrix;
	LOG(INFO) << "Generating VAO..." << endl;
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	LOG(INFO) << "Generating VAO... DONE" << endl;
	
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
	initRenderingBuffers();

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	// INVERT DEPTH BUFFER
	glDepthRange(1, 0);
	glClearDepth(0.0f);
	glDepthFunc(GL_GREATER);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.0, 0.0, 0.0, 1.0);


	// Create and compile our GLSL program from the shaders
	char* vertexFilePath = (char*)"assets/opengl/Silhouette.vs";
	char* fragmentFilePath = (char*)"assets/opengl/Silhouette.fs";

	m_SilhouetteShaderProgram = LoadShaders(vertexFilePath, fragmentFilePath);
	// Get a handle for our "MVP" uniform
	m_SilhouetteMatrixId = glGetUniformLocation(m_SilhouetteShaderProgram, "uMVPMatrix");
	m_SilhouetteAlphaId = glGetUniformLocation(m_SilhouetteShaderProgram, "uAlpha");
	m_SilhouetteColorId = glGetUniformLocation(m_SilhouetteShaderProgram, "uColor");

	// Phongblinn Shader
	m_PhongblinnShaderProgram		= LoadShaders((char*)"assets/opengl/Phongblinn.vs", (char*)"assets/opengl/Phongblinn.fs");
	m_PhongblinnMVPMatrixID			= glGetUniformLocation(m_PhongblinnShaderProgram, "uMVPMatrix");
	m_PhongblinnMVMatrixID			= glGetUniformLocation(m_PhongblinnShaderProgram, "uMVMatrix");
	m_PhongblinnNormalMatrixID		= glGetUniformLocation(m_PhongblinnShaderProgram, "uNormalMatrix");
	m_PhongblinnLightPosition1ID	= glGetUniformLocation(m_PhongblinnShaderProgram, "uLightPosition1");
	m_PhongblinnLightPosition2ID	= glGetUniformLocation(m_PhongblinnShaderProgram, "uLightPosition2");
	m_PhongblinnLightPosition3ID	= glGetUniformLocation(m_PhongblinnShaderProgram, "uLightPosition3");
	m_PhongblinnShininessID			= glGetUniformLocation(m_PhongblinnShaderProgram, "uShininess");
	m_PhongblinnAlphaID				= glGetUniformLocation(m_PhongblinnShaderProgram, "uAlpha");
	m_PhongblinnColorID				= glGetUniformLocation(m_PhongblinnShaderProgram, "uColor");



	angle = 0;

	lightPosition = cv::Vec3f(0, 0, 0);

	m_IsInitialized = true;
}

float View::getZNear()
{
	return zn;
}

float View::getZFar()
{
	return zf;
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

int View::getLevel()
{
	return currentLevel;
}

bool View::initRenderingBuffers()
{
	glBindVertexArray(m_VAO);

	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

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

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureID, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "error creating rendering buffers" << endl;
		return false;
	}
	return true;
}


static bool PtInFrame(const cv::Vec2f &pt, int width, int height)
{
	return (pt(0) < width && pt(1) < height && pt(0) >= 0 && pt(1) >= 0);
}

void View::RenderSilhouette(shared_ptr<Model> model, GLenum polyonMode, bool invertDepth, const std::vector<cv::Point3f> &colors, bool drawAll)
{
	glBindVertexArray(m_VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, width, height);
	if (invertDepth)
	{
		glClearDepth(1.0f);
		glDepthFunc(GL_LESS);
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (model->isInitialized() || drawAll)
	{
		Matx44f pose = model->getPose();
		Matx44f normalization = model->getNormalization();

		Matx44f modelViewMatrix = lookAtMatrix * (pose * normalization);

		Matx44f modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

		// Bind back to the main framebuffer
        glUseProgram(m_SilhouetteShaderProgram);
        
		// visualize camera
        glUniformMatrix4fv(m_SilhouetteMatrixId, 1, GL_TRUE, modelViewProjectionMatrix.val);
		glUniform1f(m_SilhouetteAlphaId, 1.0f);

		glm::vec3 color = glm::vec3((float)(model->getModelID()) / 255.0f, 0.0f, 0.0f);
		glUniform3fv(m_SilhouetteColorId, 1, &color[0]);

		glPolygonMode(GL_FRONT_AND_BACK, polyonMode);

		model->draw(m_SilhouetteShaderProgram);
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

void View::RenderShaded(std::shared_ptr<Model> model, GLenum polyonMode, const cv::Point3f color, bool drawAll)
{
	glBindVertexArray(m_VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

	glViewport(0, 0, width, height);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (model->isInitialized() || drawAll)
	{
		Matx44f pose = model->getPose();
		Matx44f normalization = model->getNormalization();

		Matx44f modelViewMatrix = lookAtMatrix * (pose * normalization);

		Matx33f normalMatrix = modelViewMatrix.get_minor<3, 3>(0, 0).inv().t();

		Matx44f modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

		glUseProgram(m_PhongblinnShaderProgram);
		glUniformMatrix4fv(m_PhongblinnMVMatrixID, 1, GL_TRUE, modelViewMatrix.val);
		glUniformMatrix4fv(m_PhongblinnMVPMatrixID, 1, GL_TRUE, modelViewProjectionMatrix.val);
		glUniformMatrix3fv(m_PhongblinnNormalMatrixID, 1, GL_TRUE, normalMatrix.val);
		auto lightPosition1 = glm::vec3(0.1, 0.1, -0.02);
		auto lightPosition2 = glm::vec3(-0.1, 0.1, -0.02);
		auto lightPosition3 = glm::vec3(0.0, 0.0, 0.1);
		glUniform3fv(m_PhongblinnLightPosition1ID, 1, &lightPosition1[0]);
		glUniform3fv(m_PhongblinnLightPosition2ID, 1, &lightPosition2[0]);
		glUniform3fv(m_PhongblinnLightPosition3ID, 1, &lightPosition3[0]);
		glUniform1f(m_PhongblinnShininessID, 100.0f);
		glUniform1f(m_PhongblinnAlphaID, 1.0f);

		glm::vec3 colorGlm = glm::vec3(color.x, color.y, color.z);
		glUniform3fv(m_PhongblinnColorID, 1, &colorGlm[0]);

		glPolygonMode(GL_FRONT_AND_BACK, polyonMode);

		model->draw(m_PhongblinnShaderProgram);
	}

	glFinish();
}


void View::ProjectBoundingBox(std::shared_ptr<Model> model, std::vector<cv::Point2f> &projections, cv::Rect &boundingRect)
{
	Vec3f lbn = model->getLBN();
	Vec3f rtf = model->getRTF();

	Vec4f Plbn = Vec4f(lbn[0], lbn[1], lbn[2], 1.0);
	Vec4f Prbn = Vec4f(rtf[0], lbn[1], lbn[2], 1.0);
	Vec4f Pltn = Vec4f(lbn[0], rtf[1], lbn[2], 1.0);
	Vec4f Plbf = Vec4f(lbn[0], lbn[1], rtf[2], 1.0);
	Vec4f Pltf = Vec4f(lbn[0], rtf[1], rtf[2], 1.0);
	Vec4f Prtn = Vec4f(rtf[0], rtf[1], lbn[2], 1.0);
	Vec4f Prbf = Vec4f(rtf[0], lbn[1], rtf[2], 1.0);
	Vec4f Prtf = Vec4f(rtf[0], rtf[1], rtf[2], 1.0);

	vector<Vec4f> points3D;
	points3D.push_back(Plbn);
	points3D.push_back(Prbn);
	points3D.push_back(Pltn);
	points3D.push_back(Plbf);
	points3D.push_back(Pltf);
	points3D.push_back(Prtn);
	points3D.push_back(Prbf);
	points3D.push_back(Prtf);

	Matx44f pose = model->getPose();
	Matx44f normalization = model->getNormalization();

	Point2f lt(FLT_MAX, FLT_MAX);
	Point2f rb(-FLT_MAX, -FLT_MAX);

	for (auto &i : points3D)
	{
		Vec4f p = calibrationMatrices[currentLevel] * pose * normalization * i;

		if (p[2] == 0)
			continue;

		Point2f p2d = Point2f(p[0] / p[2], p[1] / p[2]);
		projections.push_back(p2d);

		if (p2d.x < lt.x)
			lt.x = p2d.x;
		if (p2d.x > rb.x)
			rb.x = p2d.x;
		if (p2d.y < lt.y)
			lt.y = p2d.y;
		if (p2d.y > rb.y)
			rb.y = p2d.y;
	}

	boundingRect.x = lt.x;
	boundingRect.y = lt.y;
	boundingRect.width = rb.x - lt.x;
	boundingRect.height = rb.y - lt.y;
}

Mat View::DownloadFrame(View::FrameType type)
{
	glBindVertexArray(m_VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
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
	// glBindVertexArray(0);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return res;
}