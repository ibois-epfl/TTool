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

#include <memory>

#include "object3d.hh"
#include "tclc_histograms.hh"
#include "template_view.hh"

using namespace std;
using namespace cv;

bool sortDistance(std::pair<float, int> a, std::pair<float, int> b)
{
    return a.first < b.first;
}

void ttool::tslet::Object3D::Init(float qualityThreshold, std::vector<float> &templateDistances) {
    this->trackingLost = false;
    this->qualityThreshold = qualityThreshold;
    this->templateDistances = templateDistances;
    this->numDistances = (int)templateDistances.size();
    this->tclcHistograms = NULL;
    
    // icosahedron geometry for generating the base templates
    baseIcosahedron.push_back(Vec3f(0, 1, 1.61803));
    baseIcosahedron.push_back(Vec3f(1, 1.61803, 0));
    baseIcosahedron.push_back(Vec3f(-1, 1.61803, 0));
    baseIcosahedron.push_back(Vec3f(0, 1, -1.61803));
    baseIcosahedron.push_back(Vec3f(1.61803, 0, -1));
    baseIcosahedron.push_back(Vec3f(0, -1, -1.61803));
    baseIcosahedron.push_back(Vec3f(-1.61803, 0, -1));
    baseIcosahedron.push_back(Vec3f(-1, -1.61803, 0));
    baseIcosahedron.push_back(Vec3f(-1.61803, 0, 1));
    baseIcosahedron.push_back(Vec3f(0, -1, 1.61803));
    baseIcosahedron.push_back(Vec3f(1.61803, 0, 1));
    baseIcosahedron.push_back(Vec3f(1, -1.61803, 0));
    
    // sub-divided icosahedron geometry for generating the neighboring templates
    subdivIcosahedron.push_back(Vec3f(0, 1.90811, 0));
    subdivIcosahedron.push_back(Vec3f(-0.589637, 1.54369, 0.954053));
    subdivIcosahedron.push_back(Vec3f(0.589637, 1.54369, 0.954053));
    subdivIcosahedron.push_back(Vec3f(0.589637, 1.54369, -0.954053));
    subdivIcosahedron.push_back(Vec3f(-0.589637, 1.54369, -0.954053));
    subdivIcosahedron.push_back(Vec3f(0, 0, -1.90811));
    subdivIcosahedron.push_back(Vec3f(0.954053, 0.589637, -1.54369));
    subdivIcosahedron.push_back(Vec3f(0.954053, -0.589637, -1.54369));
    subdivIcosahedron.push_back(Vec3f(-0.954053, -0.589637, -1.54369));
    subdivIcosahedron.push_back(Vec3f(-0.954053, 0.589637, -1.54369));
    subdivIcosahedron.push_back(Vec3f(-1.90811, 0, 0));
    subdivIcosahedron.push_back(Vec3f(-1.54369, -0.954053, -0.589637));
    subdivIcosahedron.push_back(Vec3f(-1.54369, -0.954053, 0.589637));
    subdivIcosahedron.push_back(Vec3f(-1.54369, 0.954053, 0.589637));
    subdivIcosahedron.push_back(Vec3f(-1.54369, 0.954053, -0.589637));
    subdivIcosahedron.push_back(Vec3f(0, 0, 1.90811));
    subdivIcosahedron.push_back(Vec3f(-0.954053, 0.589637, 1.54369));
    subdivIcosahedron.push_back(Vec3f(-0.954053, -0.589637, 1.54369));
    subdivIcosahedron.push_back(Vec3f(0.954053, -0.589637, 1.54369));
    subdivIcosahedron.push_back(Vec3f(0.954053, 0.589637, 1.54369));
    subdivIcosahedron.push_back(Vec3f(1.90811, 0, 0));
    subdivIcosahedron.push_back(Vec3f(1.54369, -0.954053, 0.589637));
    subdivIcosahedron.push_back(Vec3f(1.54369, -0.954053, -0.589637));
    subdivIcosahedron.push_back(Vec3f(1.54369, 0.954053, -0.589637));
    subdivIcosahedron.push_back(Vec3f(1.54369, 0.954053, 0.589637));
    subdivIcosahedron.push_back(Vec3f(0, -1.90811, 0));
    subdivIcosahedron.push_back(Vec3f(-0.589637, -1.54369, -0.954053));
    subdivIcosahedron.push_back(Vec3f(0.589637, -1.54369, -0.954053));
    subdivIcosahedron.push_back(Vec3f(0.589637, -1.54369, 0.954053));
    subdivIcosahedron.push_back(Vec3f(-0.589637, -1.54369, 0.954053));
    subdivIcosahedron.push_back(Vec3f(-1.00074, 1.61923, 0));
    subdivIcosahedron.push_back(Vec3f(0, 1.00074, 1.61923));
    subdivIcosahedron.push_back(Vec3f(1.00074, 1.61923, 0));
    subdivIcosahedron.push_back(Vec3f(0, 1.00074, -1.61923));
    subdivIcosahedron.push_back(Vec3f(1.61923, 0, -1.00074));
    subdivIcosahedron.push_back(Vec3f(0, -1.00074, -1.61923));
    subdivIcosahedron.push_back(Vec3f(-1.61923, 0, -1.00074));
    subdivIcosahedron.push_back(Vec3f(-1.00074, -1.61923, 0));
    subdivIcosahedron.push_back(Vec3f(-1.61923, 0, 1.00074));
    subdivIcosahedron.push_back(Vec3f(0, -1.00074, 1.61923));
    subdivIcosahedron.push_back(Vec3f(1.61923, 0, 1.00074));
    subdivIcosahedron.push_back(Vec3f(1.00074, -1.61923, 0));
}

ttool::tslet::Object3D::Object3D(const std::string objFilename, const cv::Matx44f& Ti, float scale, float qualityThreshold, std::vector<float> &templateDistances) 
	: Model(objFilename, Ti, scale)
{
	Init(qualityThreshold, templateDistances);
}

ttool::tslet::Object3D::Object3D(const string objFilename, float tx, float ty, float tz, float alpha, float beta, float gamma, float scale, float qualityThreshold,  vector<float> &templateDistances) 
	: Model(objFilename, tx, ty, tz, alpha, beta, gamma, scale)
{
	Init(qualityThreshold, templateDistances);
}


ttool::tslet::Object3D::~Object3D()
{
    baseTemplates.clear();
    
    neighboringTemplates.clear();
}


std::shared_ptr<ttool::tslet::TCLCHistograms> ttool::tslet::Object3D::getTCLCHistograms()
{
    return tclcHistograms;
}

void ttool::tslet::Object3D::SetTCLCHistograms(std::shared_ptr<ttool::tslet::TCLCHistograms> histograms) {
    tclcHistograms = histograms;
}

void ttool::tslet::Object3D::generateTemplates()
{
    int numLevels = 4;
    
    int numBaseRotations = 4;
    
    // create all base templates
    for(int i = 0; i < baseIcosahedron.size(); i++)
    {
        Vec3f v = baseIcosahedron[i];
        
        float r = norm(v);
        float alpha = acos(v[1]/r)*180.0f/float(CV_PI) - 90.0f;
        float beta = atan2(v[0], v[2])*180.0f/float(CV_PI);
        
        for(int gamma = 0; gamma < 360; gamma += 90)
        {
            for(int d = 0; d < numDistances; d++)
            {
                baseTemplates.push_back(std::make_shared<TemplateView>(std::static_pointer_cast<ttool::tslet::Object3D>(shared_from_this()), alpha, beta, gamma, templateDistances[d], numLevels, true));
            }
        }
    }
    
    int gamma2Precision = 30;
    
    // create all neighboring templates
    for(int i = 0; i < subdivIcosahedron.size(); i++)
    {
        Vec3f v = subdivIcosahedron[i];
        
        float r = norm(v);
        float alpha = acos(v[1]/r)*180.0f/float(CV_PI) - 90.0f;
        float beta = atan2(v[0], v[2])*180.0f/float(CV_PI);
        
        for(int gamma = 0; gamma < 360; gamma += gamma2Precision)
        {
            for(int d = 0; d < numDistances; d++)
            {
                neighboringTemplates.push_back(std::make_shared<TemplateView>(std::static_pointer_cast<ttool::tslet::Object3D>(shared_from_this()), alpha, beta, gamma, templateDistances[d], numLevels, true));
            }
        }
    }
    
    int gamma2Steps = 360/gamma2Precision;
    
    // associate each base template with its corresponding neighboring templates
    for(int i = 0; i < baseIcosahedron.size(); i++)
    {
        Vec3f v1 = baseIcosahedron[i];
        
        vector<pair<float, int> > distanceMap;
        
        for(int j = 0; j < subdivIcosahedron.size(); j++)
        {
            Vec3f v2 = subdivIcosahedron[j];
            
            float d = norm(v1 - v2);
            distanceMap.push_back(pair<float, int>(d, j));
        }
        
        sort(distanceMap.begin(), distanceMap.end(), sortDistance);
        
        for(int n = 0; n < 6; n++)
        {
            for(int g = 0; g < numBaseRotations; g++)
            {
                for(int d = 0; d < numDistances; d++)
                {
                    std::shared_ptr<TemplateView> kv = baseTemplates[i*numBaseRotations*numDistances + numDistances*g + d];
                    float gamma1 = kv->getGamma();
                    
                    int g2 = gamma1/gamma2Precision;
                    
                    kv->addNeighborTemplate(neighboringTemplates[distanceMap[n].second*gamma2Steps*numDistances + g2*numDistances + d]);
                    
                    int g3 = (g2+gamma2Steps+1)%gamma2Steps;
                    kv->addNeighborTemplate(neighboringTemplates[distanceMap[n].second*gamma2Steps*numDistances + g3*numDistances + d]);
                    
                    int g4 = (g2+gamma2Steps-1)%gamma2Steps;
                    kv->addNeighborTemplate(neighboringTemplates[distanceMap[n].second*gamma2Steps*numDistances + g4*numDistances + d]);
                }
            }
        }
    }
    
    // reset the model to its prescribed initial pose
    // Model::reset();
}


vector<std::shared_ptr<TemplateView>> ttool::tslet::Object3D::getTemplateViews()
{
    return baseTemplates;
}


int ttool::tslet::Object3D::getNumDistances()
{
    return numDistances;
}


void ttool::tslet::Object3D::reset()
{
    Model::reset();
    
    if (tclcHistograms)
        this->tclcHistograms = NULL;
        // tclcHistograms->clear();
    
    trackingLost = false;
}
