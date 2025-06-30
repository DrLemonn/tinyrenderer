#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "Model.h"
#include "tgaimage.h"


extern const float PI;


struct IShader{
    virtual ~IShader() = default;
    virtual Vector4f vertex(int face_index, int vert_index) = 0;
    virtual bool fragment(Vector3f bar, TGAColor& color) = 0;
};

struct GouraudShader : IShader{
    // --- "Uniforms" ---
    Model* m = nullptr;
    Matrix4f modelMatrix;
    Matrix4f viewMatrix;
    Matrix4f projectionMatrix;
    Matrix4f viewportMatrix;
    Vector3f lightDir;

    // --- "Varying" ---
    Vector3f varing_intensity;

    Vector4f vertex(int face_index, int vert_index) override {
        Vector3f v_pos = m->vert[(m->vertex_inds)[face_index][vert_index]];
        Vector3f normal = m->norm[(m->norm_inds)[face_index][vert_index]];

        varing_intensity[vert_index] = std::max(0.f, normal * lightDir);
        return viewportMatrix * projectionMatrix * viewMatrix * modelMatrix * toVec4(v_pos);
    }

    bool fragment(Vector3f bar, TGAColor& color) override {
        float intensity = bar * varing_intensity;
        color = TGAColor(255, 255, 255) * intensity; // well duh
        return false; 
    }
};

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
    void rasterize_triangle(Vector4f a, Vector4f b, Vector4f c, IShader& shader);
    void write_tga_file(std::string filename);
};



