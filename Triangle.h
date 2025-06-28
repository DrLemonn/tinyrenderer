#pragma once
#include "geometry.h"


struct Triangle
{
    Vector3f vertex[3];
    Vector3f normal[3]; // 法向量
    Vector2f uv[3]; // 纹理坐标
    Vector3f color[3]; // 顶点颜色

    Triangle() {}

    Vector3f a(){return vertex[0];}
    Vector3f b(){return vertex[1];}
    Vector3f c(){return vertex[2];}    
    
    void setVertex(int i, const Vector3f& v) {
        assert(i >= 0 && i < 3);
        vertex[i] = v;
    }

    void setNormal(int i, const Vector3f& n) {
        assert(i >= 0 && i < 3);
        normal[i] = n;
    }

    void setUV(int i, const Vector2f& uvCoord) {
        assert(i >= 0 && i < 3);
        uv[i] = uvCoord;
    }

    void setColor(int i, const Vector3f& c) {
        assert(i >= 0 && i < 3);
        if((c[0]<0.0) || (c[0]>255.) ||
            (c[1]<0.0) || (c[1]>255.) ||
            (c[2]<0.0) || (c[2]>255.)) {
            fprintf(stderr, "ERROR! Invalid color values");
            fflush(stderr);
            exit(-1);
        }
        color[i] = c;
    }


};

