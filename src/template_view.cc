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

#include "template_view.hh"

using namespace std;
using namespace cv;

TemplateView::TemplateView(std::shared_ptr<ttool::tslet::Object3D> object, float alpha, float beta, float gamma, float distance, int numLevels, bool generateNeighbors)
{
    T_cm = Transformations::translationMatrix(0, 0, distance)*Transformations::rotationMatrix(gamma, Vec3f(0, 0, 1))*Transformations::rotationMatrix(alpha, Vec3f(1, 0, 0))*Transformations::rotationMatrix(beta, Vec3f(0, 1, 0));
    
    object->setPose(T_cm);
    
    view = View::Instance();
    
    view->SetLevel(0);
    view->RenderSilhouette(object, GL_FILL, false);
    
    Mat mask0 = view->DownloadFrame(View::MASK);
    Mat depth0 = view->DownloadFrame(View::DEPTH);
    
    Matx33f K = view->GetCalibrationMatrix().get_minor<3, 3>(0, 0);
    
    float zNear = view->GetZNear();
    float zFar = view->GetZFar();
    
    // std::cout << "template_view" << std::endl;
    std::shared_ptr<TCLCHistograms> tclcHistograms = object->getTCLCHistograms();
    
    int m_id = object->getModelID();
    
    _alpha = alpha;
    _beta = beta;
    _gamma = gamma;
    
    _distance = distance;
    
    _numLevels = numLevels;
    
    centersIDsPyramid.resize(_numLevels);
    roiPyramid.resize(_numLevels);
    etaFPyramid.resize(_numLevels);
    maskPyramid.resize(_numLevels);
    sdtPyramid.resize(_numLevels);
    heavisidePyramid.resize(_numLevels);
    pixelDataPyramid.resize(_numLevels);
    
    ttool::tslet::SignedDistanceTransform2D SDT2D(8.0f);
    
    Size maxSize = mask0.size();
    
    for(int level = 2; level < _numLevels; level++)
    {
        int scale = pow(2, level);
  
        tclcHistograms->updateCentersAndIds(mask0/255*m_id, depth0, K, zNear, zFar, 0);
    
        std::vector<cv::Point3i> centersIDs = tclcHistograms->getCentersAndIDs();
        
        centersIDsPyramid[level] = centersIDs;
    
        int offset = tclcHistograms->getRadius()/pow(2, level);
    
        Rect roi = computeBoundingBox(centersIDs, offset, level, Size(maxSize.width/scale, maxSize.height/scale));
        
        roiPyramid[level] = roi;
    
        view->SetLevel(level);
        view->RenderSilhouette(object, GL_FILL, false);
        
        Mat mask = view->DownloadFrame(View::MASK);
        mask = mask(roi).clone();
        
        etaFPyramid[level] = countNonZero(mask);
        
        maskPyramid[level] = mask*255;
        
        Mat sdt, xyPos;
        SDT2D.computeTransform(mask, sdt, xyPos, 8);
    
        sdtPyramid[level] = sdt;
        
        Mat heaviside;
        parallel_for_(cv::Range(0, 8), Parallel_For_convertToHeaviside(sdt, heaviside, 8));
        
        heavisidePyramid[level] = heaviside;
        
        compressTemplateData(centersIDs, heaviside, roi, tclcHistograms->getRadius(), level);
    }
}


TemplateView::~TemplateView()
{
    for(int i = 0; i < pixelDataPyramid.size(); i++)
    {
        for(int j = 0; j < pixelDataPyramid[i].size(); j++)
        {
            delete[] pixelDataPyramid[i][j].ids;
        }
        pixelDataPyramid[i].clear();
    }
    pixelDataPyramid.clear();
}

Matx44f TemplateView::getPoseMatrix()
{
    return T_cm;
}

float TemplateView::getAlpha()
{
    return _alpha;
}

float TemplateView::getBeta()
{
    return _beta;
}

float TemplateView::getGamma()
{
    return _gamma;
}

float TemplateView::getDistance()
{
    return _distance;
}

int TemplateView::getEtaF(int level)
{
    return etaFPyramid[level];
}

Mat TemplateView::getMask(int level)
{
    return maskPyramid[level];
}

Mat TemplateView::getSDT(int level)
{
    return sdtPyramid[level];
}

Mat TemplateView::getHeaviside(int level)
{
    return heavisidePyramid[level];
}

Rect TemplateView::getROI(int level)
{
    return roiPyramid[level];
}

Point3f TemplateView::getCurrentOffset(int level)
{
    return currentOffset;
}

void TemplateView::setCurrentOffset(Point3f &offset, int level)
{
    currentOffset = offset;
}

vector<Point3i> TemplateView::getCentersAndIDs(int level)
{
    return centersIDsPyramid[level];
}


std::vector<PixelData>& TemplateView::getCompressedPixelData(int level)
{
    return pixelDataPyramid[level];
}

void TemplateView::addNeighborTemplate(std::shared_ptr<TemplateView> kv)
{
    neighbors.push_back(kv);
}

std::vector<std::shared_ptr<TemplateView>> TemplateView::getNeighborTemplates()
{
    return neighbors;
}

void TemplateView::compressTemplateData(const std::vector<cv::Point3i>& centersIDs, const cv::Mat &heaviside, const cv::Rect& roi, int radius, int level)
{
    int numHistograms = (int)centersIDs.size();
    int scale = pow(2, level);
    int radius2 = radius*radius;
    
    float *hsData = (float*)heaviside.ptr<float>();
    
    for(int j = 0; j < roi.height; j++)
    {
        int idx = j*roi.width;
        
        for(int i = 0; i < roi.width; i++, idx++)
        {
            float hsVal = hsData[idx];
            
            if(hsVal >= 0.0f)
            {
                vector<int> ids;
                for(int h = 0; h < numHistograms; h++)
                {
                    cv::Point3i centerID = centersIDs[h];
                    int dx = centerID.x - scale*(i+roi.x + 0.5f);
                    int dy = centerID.y - scale*(j+roi.y + 0.5f);
                    
                    int distance = dx*dx + dy*dy;
                    
                    if(distance <= radius2)
                    {
                        ids.push_back(centerID.z);
                    }
                }
                
                if(ids.size() > 1)
                {
                    PixelData pixelData;
                    pixelData.x = i;
                    pixelData.y = j;
                    pixelData.hsVal = hsVal;
                    pixelData.ids_size = (int)ids.size();
                    pixelData.ids = new int[pixelData.ids_size];
                    
                    for(int k = 0; k < ids.size(); k++)
                    {
                        pixelData.ids[k] = ids[k];
                    }
                    
                    pixelDataPyramid[level].push_back(pixelData);
                }
            }
        }
    }
}

cv::Rect TemplateView::computeBoundingBox(const std::vector<cv::Point3i> &centersIDs, int offset, int level, const cv::Size &maxSize)
{
    int minX = INT_MAX, minY = INT_MAX;
    int maxX = -1, maxY = -1;
    
    int scale = pow(2, level);
    
    for(int i = 0; i < centersIDs.size(); i++)
    {
        Point3i p = centersIDs[i];
        int x = p.x/scale;
        int y = p.y/scale;
        
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }
    
    minX -= offset;
    minY -= offset;
    maxX += offset;
    maxY += offset;
    
    if(minX < 0) minX = 0;
    if(minY < 0) minY = 0;
    if(maxX > maxSize.width) maxX = maxSize.width;
    if(maxY > maxSize.height) maxY = maxSize.height;
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}
