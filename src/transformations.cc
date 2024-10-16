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

#include "transformations.hh"

cv::Matx44f ttool::utils::Transformations::scaleMatrix(float s)
{
    return scaleMatrix(s,s,s);
}

cv::Matx44f ttool::utils::Transformations::scaleMatrix(float sx, float sy, float sz)
{
    return cv::Matx44f(sx, 0,  0,  0,
                   0,  sy, 0,  0,
                   0,  0,  sz, 0,
                   0,  0,  0,  1);
}

cv::Matx44f ttool::utils::Transformations::translationMatrix(const cv::Vec3f tvec)
{
    return translationMatrix(tvec[0], tvec[1], tvec[2]);
}

cv::Matx44f ttool::utils::Transformations::translationMatrix(float tx, float ty, float tz)
{
    return cv::Matx44f(1, 0, 0, tx,
                   0, 1, 0, ty,
                   0, 0, 1, tz,
                   0, 0, 0, 1);
}


cv::Matx44f ttool::utils::Transformations::rotationMatrix(float angle, cv::Vec3f axis)
{
    angle = (angle/180)*CV_PI;
    
    float s = sin(angle);
    float c = cos(angle);
    float mc = 1.0f - c;
    
    float len = norm(axis);
    
    if (len == 0)
    {
        // avoid zero division error
        return cv::Matx44f::eye();
    }
    
    axis /= len;
    float x = axis[0];
    float y = axis[1];
    float z = axis[2];
    
    return cv::Matx44f(x * x * mc + c,     x * y * mc - z * s, x * z * mc + y * s, 0,
                   x * y * mc + z * s, y * y * mc + c,     y * z * mc - x * s, 0,
                   x * z * mc - y * s, y * z * mc + x * s, z * z * mc + c,     0,
                   0,                  0,                  0,                  1);
}

cv::Matx44f ttool::utils::Transformations::lookAtMatrix(float ex, float ey, float ez, float cx, float cy, float cz, float ux, float uy, float uz)
{
    cv::Vec3f eye(ex,ey,ez);
    cv::Vec3f center(cx,cy,cz);
    cv::Vec3f up(ux,uy,uz);
    
    up /= norm(up);
    
    cv::Vec3f f = center-eye;
    f /= norm(f);
    
    cv::Vec3f s = f.cross(up);
    s /= norm(s);
    
    cv::Vec3f u = s.cross(f);
    u /= norm(u);
    
    return cv::Matx44f(s[0],  s[1],  s[2], -s.dot(eye),
                   u[0],  u[1],  u[2], -u.dot(eye),
                  -f[0], -f[1], -f[2],  f.dot(eye),
                   0,     0,     0,     1);
}

cv::Matx44f ttool::utils::Transformations::perspectiveMatrix(float fovy, float aspect, float zNear, float zFar)
{
    fovy = fovy*(float)CV_PI/180.0;
    float focal = 1.0/tan(fovy/2.0);
    
    float n = zNear;
    float f = zFar;
    
    return cv::Matx44f(focal/aspect, 0,      0,           0,
                   0,            focal,  0,           0,
                   0,            0,     (f+n)/(n-f), (2*f*n)/(n-f),
                   0,            0,     -1,           0);
}

cv::Matx44f ttool::utils::Transformations::perspectiveMatrix(const cv::Matx33f& K, int width, int height, float zNear, float zFar, bool flipY)
{
    float fx = K(0,0);
    float fy = K(1,1);
    
    float cx = K(0,2);
    float cy = K(1,2);
    
    float w = width;
    float h = height;
    
    float n = zNear;
    float f = zFar;
    
    if(flipY)
    {
        return cv::Matx44f(2*fx/w, 0,        1-2*cx/w,    0,
                       0,      -2*fy/h,  1-2*cy/h,    0,
                       0,      0,       (f+n)/(n-f), (2*f*n)/(n-f),
                       0,      0,        -1,          0);
    }
    
    return cv::Matx44f(2*fx/w, 0,       1-2*cx/w,    0,
                   0,      2*fy/h,  2*cy/h-1,    0,
                   0,      0,      (f+n)/(n-f), (2*f*n)/(n-f),
                   0,      0,       -1,          0);
}

cv::Matx33f ttool::utils::Transformations::axiator(cv::Vec3f a)
{
    float a1 = a[0];
    float a2 = a[1];
    float a3 = a[2];
    
    return cv::Matx33f(0,  -a3,  a2,
                   a3,  0,  -a1,
                   -a2,  a1,  0);
}

cv::Matx44f ttool::utils::Transformations::exp(cv::Matx61f xi)
{
    cv::Matx44f T = cv::Matx44f::eye();
    
    // rotational part of the twist coordinates (orientation)
    cv::Vec3f r = cv::Vec3f(xi(0, 0), xi(1, 0), xi(2, 0));
    
    // translational part of the twist coordinates (velocity)
    cv::Vec3f v = cv::Vec3f(xi(3, 0), xi(4, 0), xi(5, 0));
    
    // angle of the twist/rotation
    float theta = norm(r);
    
    // return the identity group element for theta == 0, as there is no motion
    if(abs(theta) < FLT_EPSILON)
    {
        return T;
    }
    else
    {
        // compute the rotation matrix as the matrix exponential of r
        cv::Matx33f R;
        Rodrigues(r, R);
        
        // copy R to final pose
        T(0, 0) = R(0, 0); T(0, 1) = R(0, 1); T(0, 2) = R(0, 2);
        T(1, 0) = R(1, 0); T(1, 1) = R(1, 1); T(1, 2) = R(1, 2);
        T(2, 0) = R(2, 0); T(2, 1) = R(2, 1); T(2, 2) = R(2, 2);
        
        // compute the translation vector t
        cv::Matx33f I = cv::Matx33f::eye();
        cv::Vec3f w = r/theta;
        cv::Matx33f w_x = ttool::utils::Transformations::axiator(w);
        v /= theta;
        
        cv::Vec3f t = (I - R)*w_x*v + w*w.t()*v*theta;
        
        // copy t to final pose
        T(0, 3) = t[0];
        T(1, 3) = t[1];
        T(2, 3) = t[2];
    }
    
    return T;
}
