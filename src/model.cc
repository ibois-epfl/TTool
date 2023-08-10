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

#include "model.hh"

#include <limits>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include "util.hh" // for ttool::utils::IsFileExist

ttool::tslet::Model::Model(const std::string modelFilename, float tx, float ty, float tz, float alpha, float beta, float gamma, float scale)
{
	cv::Matx44f Ti = ttool::utils::Transformations::translationMatrix(tx, ty, tz)
	*ttool::utils::Transformations::rotationMatrix(alpha, cv::Vec3f(1, 0, 0))
	*ttool::utils::Transformations::rotationMatrix(beta, cv::Vec3f(0, 1, 0))
	*ttool::utils::Transformations::rotationMatrix(gamma, cv::Vec3f(0, 0, 1))
	*cv::Matx44f::eye();

	Init(modelFilename, Ti, scale);
}

ttool::tslet::Model::Model(const std::string modelFilename, const cv::Matx44f& Ti, float scale) {
	Init(modelFilename, Ti, scale);
}

void ttool::tslet::Model::Init(const std::string modelFilename, const cv::Matx44f& Ti, float scale) {
	m_id = 0;
    
	initialized = false;
    
	buffersInitialsed = false;
    
	T_i = Ti;
    
	T_cm = T_i;
  T_pm = T_i;

	scaling = scale;
    
	T_n = cv::Matx44f::eye();
    
	hasNormals = false;
    
  loadModel(modelFilename);
}

ttool::tslet::Model::~Model()
{
    vertices.clear();
    normals.clear();
    
    indices.clear();
    offsets.clear();
}

void ttool::tslet::Model::initBuffers()
{
  	glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    buffersInitialsed = true;
}


void ttool::tslet::Model::initialize()
{
    initialized = true;
}


bool ttool::tslet::Model::isInitialized()
{
    return initialized;
}


void ttool::tslet::Model::draw(GLint program, GLint primitives)
{
    auto aPosition = glGetAttribLocation(program, "aPosition");
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(aPosition);

    auto aNormal = glGetAttribLocation(program, "aNormal");
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(aNormal);
    
    auto aColor = glGetAttribLocation(program, "aColor");
    glVertexAttribPointer(aColor, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(aColor);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    for (uint i = 0; i < offsets.size() - 1; i++) {
        GLuint size = offsets.at(i + 1) - offsets.at(i);
        GLuint offset = offsets.at(i);

        glDrawElements(primitives, size, GL_UNSIGNED_INT, (GLvoid*)(offset*sizeof(GLuint)));
    }
}


cv::Matx44f ttool::tslet::Model::getPose()
{
    return T_cm;
}

void ttool::tslet::Model::setPose(const cv::Matx44f &T_cm)
{
    this->T_cm = T_cm;
}

cv::Matx44f ttool::tslet::Model::getPrePose()
{
    return T_pm;
}

void ttool::tslet::Model::setPrePose(const cv::Matx44f &T_cm)
{
    this->T_pm = T_cm;
}

void ttool::tslet::Model::setInitialPose(const cv::Matx44f &Ti)
{
    this->T_i = Ti;
}

cv::Matx44f ttool::tslet::Model::getNormalization()
{
    return T_n;
}


cv::Vec3f ttool::tslet::Model::getLBN()
{
    return lbn;
}

cv::Vec3f ttool::tslet::Model::getRTF()
{
    return rtf;
}

float ttool::tslet::Model::getScaling() {
    
    return scaling;
}


std::vector<glm::vec3> ttool::tslet::Model::getVertices()
{
    return vertices;
}

std::vector<glm::vec3> ttool::tslet::Model::getSimpleVertices()
{
    return svertices;
}

int ttool::tslet::Model::getNumVertices()
{
    return (int)vertices.size();
}

int ttool::tslet::Model::getNumSimpleVertices()
{
    return (int)svertices.size();
}

int ttool::tslet::Model::getModelID()
{
    return m_id;
}


void ttool::tslet::Model::setModelID(int i)
{
    m_id = i;
}


void ttool::tslet::Model::reset()
{
    initialized = false;
    T_cm = T_i;
}

void IdenInsert(std::vector<aiVector3D>& verts, const aiVector3D& vert) {
  for (int i = 0; i < verts.size(); ++i) {
    if (verts[i] == vert)
      return;
  }

  verts.push_back(vert);
}

void IdentAdd(std::vector<cv::Vec3f>& vertices, aiVector3D* aiv, unsigned int num) {
  vertices.clear();

  std::vector<aiVector3D> setv;

  for (int i = 0; i < num; ++i) {
    IdenInsert(setv, aiv[i]);
  }

  for (auto vert : setv) {
    vertices.push_back(cv::Vec3f(vert.x, vert.y, vert.z));
  }
}

void ttool::tslet::Model::loadModel(const std::string modelFilename)
{
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(modelFilename, aiProcessPreset_TargetRealtime_Fast);
    
    aiMesh *mesh = scene->mMeshes[0];
    
    hasNormals = mesh->HasNormals();
    
    float inf = std::numeric_limits<float>::infinity();
    lbn = cv::Vec3f(inf, inf, inf);
    rtf = cv::Vec3f(-inf, -inf, -inf);
    
    for(int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace f = mesh->mFaces[i];

        indices.push_back(f.mIndices[0]);
        indices.push_back(f.mIndices[1]);
        indices.push_back(f.mIndices[2]);
    }
    
    for(int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D v = mesh->mVertices[i];
        
        glm::vec3 p(v.x, v.y, v.z);
        
        // compute the 3D bounding box of the model
        if (p[0] < lbn[0]) lbn[0] = p[0];
        if (p[1] < lbn[1]) lbn[1] = p[1];
        if (p[2] < lbn[2]) lbn[2] = p[2];
        if (p[0] > rtf[0]) rtf[0] = p[0];
        if (p[1] > rtf[1]) rtf[1] = p[1];
        if (p[2] > rtf[2]) rtf[2] = p[2];
        
        vertices.push_back(p);
    }

    if(hasNormals)
    {
        for(int i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D n = mesh->mNormals[i];
            
            glm::vec3 vn = glm::vec3(n.x, n.y, n.z);
            
            normals.push_back(vn);
        }
    }
    
    offsets.push_back(0);
    offsets.push_back(mesh->mNumFaces*3);
    
    // the center of the 3d bounding box
    cv::Vec3f bbCenter = (rtf + lbn) / 2.0f;
    // compute a normalization transform that moves the object to the center of its bounding box and scales it according to the prescribed factor
    T_n = ttool::utils::Transformations::scaleMatrix(scaling)*ttool::utils::Transformations::translationMatrix(-bbCenter[0], -bbCenter[1], -bbCenter[2]);

//#define DELETE_EXTRA_VERTEX
#ifdef DELETE_EXTRA_VERTEX
    if (ttool::utils::IsFileExist(modelFilename + 's')) {
      //loadSimpleModel(modelFilename + 's');
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(modelFilename, aiProcessPreset_TargetRealtime_Fast);
      aiMesh *smesh = scene->mMeshes[0];
      IdentAdd(svertices, smesh->mVertices, smesh->mNumVertices);
    } else {
      //svertices = vertices;
      IdentAdd(svertices, mesh->mVertices, mesh->mNumVertices);
    }
#else
    if (ttool::utils::IsFileExist(modelFilename + 's')) {
      loadSimpleModel(modelFilename + 's');
    } else {
      svertices = vertices;
    }
#endif

#define ADD_EDGE_VERTEX
#ifdef ADD_EDGE_VERTEX
    if (mesh->mNumVertices < 24*3) {
      std::vector<aiVector3D> setv;

      for(int i = 0; i < mesh->mNumFaces; i++) {
        aiFace f = mesh->mFaces[i];

        aiVector3D dv;

        dv = mesh->mVertices[f.mIndices[0]] - mesh->mVertices[f.mIndices[1]];
        IdenInsert(setv, mesh->mVertices[f.mIndices[1]] + 0.2f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[1]] + 0.4f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[1]] + 0.6f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[1]] + 0.8f * dv);

        dv = mesh->mVertices[f.mIndices[1]] - mesh->mVertices[f.mIndices[2]];
        IdenInsert(setv, mesh->mVertices[f.mIndices[2]] + 0.2f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[2]] + 0.4f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[2]] + 0.6f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[2]] + 0.8f * dv);

        dv = mesh->mVertices[f.mIndices[2]] - mesh->mVertices[f.mIndices[0]];
        IdenInsert(setv, mesh->mVertices[f.mIndices[0]] + 0.2f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[0]] + 0.4f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[0]] + 0.6f * dv);
        IdenInsert(setv, mesh->mVertices[f.mIndices[0]] + 0.8f * dv);
      }

      svertices = vertices;
      int si = setv.size();

      for (auto vert : setv) {
        svertices.push_back(glm::vec3(vert.x, vert.y, vert.z));
      }
    }
#endif
}

void ttool::tslet::Model::Model::loadSimpleModel(const std::string modelFilename) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(modelFilename, aiProcessPreset_TargetRealtime_Fast);
  aiMesh *mesh = scene->mMeshes[0];
   
  for(int i = 0; i < mesh->mNumVertices; i++) {
    aiVector3D v = mesh->mVertices[i];
    glm::vec3 p(v.x, v.y, v.z);
    svertices.push_back(p);
  }
}
