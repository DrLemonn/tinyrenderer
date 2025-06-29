#pragma once
#include "tgaimage.h"
#include "geometry.h"


extern const float PI;

struct rasterizer{
    int width;
    int height;
    TGAImage framebuffer;
    std::vector<float> depthbuffer;

    rasterizer(int _width, int _height) : width{_width}, height{_height}, framebuffer{width, height, TGAImage::RGB}, depthbuffer(width * height){}

    void clear();

    int getIndex(int x, int y){
	    return y*width + x;
    }
    
    void line(int ax, int ay, int bx, int by, TGAColor color);
    Matrix4f getModelMatrix(float angle);
    Matrix4f lookAt(Vector3f eye_pos, Vector3f centre, Vector3f up);
    Matrix4f getViewportMatrix();
    Matrix4f getProjectionMatrix(float eye_fov, float aspect_ratio, float zNear, float zFar);
    void rasterize_triangle(Vector4f a, Vector4f b, Vector4f c, TGAColor color);
    void write_tga_file(std::string filename);
};



