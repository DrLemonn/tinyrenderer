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
    Matrix4f modelViewMatrix;
    Matrix4f modelViewInverTrans;
    Matrix4f projectionMatrix;
    Matrix4f viewportMatrix;
    Vector3f lightDir;
    Vector3f eyePos;

    // --- "Varying" ---
    // Vector3f varing_intensity;
    // Vector3f varing_position[3];
    Matrix3f varing_position;
    Matrix3f varing_normal;
    Matrix<2, 3, float> varing_uv;

    Vector4f vertex(int face_index, int vert_index) override {
        Vector3f v_pos = m->getVertex(face_index, vert_index);
        varing_position.set_col(vert_index, toVec3(modelViewMatrix * toVec4(v_pos)));
        varing_normal.set_col(vert_index, toVec3 (modelViewInverTrans * toVec4(m->getNormal(face_index, vert_index),0.f)));
        varing_uv.set_col(vert_index, m->getuv(face_index, vert_index)); 

        //varing_intensity[vert_index] = std::max(0.f, normal * lightDir);
        return viewportMatrix * projectionMatrix * modelViewMatrix * toVec4(v_pos);
    }

    bool fragment(Vector3f bar, TGAColor& color) override {
        // float intensity = bar * varing_intensity;
        // color = TGAColor(255, 255, 255) * intensity; // well duh
        Vector3f p = varing_position * bar;
        Vector3f n = varing_normal * bar;
        Vector2f uv = varing_uv * bar;

        Vector3f l = lightDir.normalize();
        Vector3f v = (eyePos - p).normalize();
        Vector3f h = (l + v) / 2.f;

        float amb = 0.1f;
        float diff = std::max(0.f, l * n);
        float spec = std::pow(std::max(0.f, h * n), 32);
        
        TGAColor tex = m->diffuse(uv);

        for(int i : {0, 1, 2}){
            color[i] = tex[i] * (diff + 0.6 * spec + amb);
        }

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



