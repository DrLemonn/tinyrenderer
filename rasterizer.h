#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "Model.h"
#include "tgaimage.h"


extern const float PI;
extern std::vector<float> shadowDepthBuffer;
extern const int width;
extern const int height;


struct IShader{
    virtual ~IShader() = default;
    virtual Vector4f vertex(int face_index, int vert_index) = 0;
    virtual bool fragment(Vector3f bar, TGAColor& color) = 0;
};

struct ShadowShader : IShader
{
    // --- "Uniforms" ---
    Model* m = nullptr;
    Matrix4f modelViewMatrix;
    Matrix4f orthoProjectionMatrix;
    Matrix4f viewportMatrix;

    // --- "Varying" ---
    Matrix3f varing_position;
    Vector4f vertex(int face_index, int vert_index) override {
        Vector3f v_pos = m->getVertex(face_index, vert_index);
        varing_position.set_col(vert_index, toVec3(modelViewMatrix * toVec4(v_pos)));

        //varing_intensity[vert_index] = std::max(0.f, normal * lightDir);
        return viewportMatrix * orthoProjectionMatrix * modelViewMatrix * toVec4(v_pos);
    }

    bool fragment(Vector3f bar, TGAColor& color) override {
        Vector3f p = varing_position * bar;
        
        color = TGAColor{static_cast<unsigned char>(255 * (p.z/(-10.f))),
            static_cast<unsigned char>(255 * (p.z/(-10.f))),
            static_cast<unsigned char>(255 * (p.z/(-10.f))),
            static_cast<unsigned char>(255 * (p.z/(-10.f)))};
        return false;
    }


};


struct GouraudShader : IShader{
    // --- "Uniforms" ---
    Model* m = nullptr;
    Matrix4f modelViewMatrix;
    Matrix4f modelViewInverTrans;
    Matrix4f projectionMatrix;
    Matrix4f viewportMatrix;
    Matrix4f lightMVPMatrix;
    Vector3f lightDir;
    Vector3f eyePos;

    // --- "Varying" ---
    // Vector3f varing_intensity;
    // Vector3f varing_position[3];
    Matrix3f varing_position;
    Matrix3f varing_normal;
    Matrix<2, 3, float> varing_uv;
    Matrix3f varing_lightTrans;

    Vector4f vertex(int face_index, int vert_index) override {
        Vector3f v_pos = m->getVertex(face_index, vert_index);
        varing_position.set_col(vert_index, toVec3(modelViewMatrix * toVec4(v_pos)));
        varing_normal.set_col(vert_index, toVec3 (modelViewInverTrans * toVec4(m->getNormal(face_index, vert_index),0.f)));
        varing_uv.set_col(vert_index, m->getuv(face_index, vert_index)); 
        varing_lightTrans.set_col(vert_index, toVec3(lightMVPMatrix * toVec4(v_pos)));
        //varing_intensity[vert_index] = std::max(0.f, normal * lightDir);
        return viewportMatrix * projectionMatrix * modelViewMatrix * toVec4(v_pos);
    }

    bool fragment(Vector3f bar, TGAColor& color) override {
        // float intensity = bar * varing_intensity;
        // color = TGAColor(255, 255, 255) * intensity; // well duh
        Vector3f p = varing_position * bar;
        Vector3f n_model = varing_normal * bar;
        Vector2f uv = varing_uv * bar;
        Vector3f lp = varing_lightTrans * bar;
        float visibility = 0.7f;
        
        Vector3f n_tan = m->normal(uv);


        // use shadow buffer
        if(lp.z > shadowDepthBuffer[int(lp.y) * width + int(lp.x)] + 1e-3){
            visibility = 0.f;
        }


        // compute TBN matrix 
        // E1 = AB E2 = AC
        // [E1 E2] = [T B][ue1  ue2]
        //                [ve1  ve2] 

        //compute [ue1  ue2]-1
        //        [ve1  ve2] 
        Matrix<2, 2, float> temp_matrix;
        temp_matrix.set_col(0, varing_uv.col(1) - varing_uv.col(0));
        temp_matrix.set_col(1, varing_uv.col(2) - varing_uv.col(0));
        temp_matrix = temp_matrix.inverse();

        Matrix<3, 2, float> TB;
        // set [E1 E2] 
        TB.set_col(0, varing_position.col(1) - varing_position.col(0));
        TB.set_col(1, varing_position.col(2) - varing_position.col(0));
        TB = TB * temp_matrix;

        Vector3f T = TB.col(0);
        Vector3f B = TB.col(1);

        // 这里需要正交化，建立n_model对应切空间中z轴的正交坐标系

        Vector3f T_ortho = (T - n_model * (T * n_model)).normalize();
        Vector3f B_ortho = cross(n_model, T_ortho).normalize();

        Matrix3f TBN;
        TBN.set_col(0, T_ortho);
        TBN.set_col(1, B_ortho);
        TBN.set_col(2, n_model);

        Vector3f n = TBN * n_tan;

        Vector3f l = lightDir.normalize();
        Vector3f v = (eyePos - p).normalize();
        Vector3f h = (l + v) / 2.f;

        float amb = 0.1f;
        float diff = std::max(0.f, l * n);
        float spec = std::pow(std::max(0.f, h * n), m->specular(uv)[0]);
        
        TGAColor tex = m->diffuse(uv);

        for(int i : {0, 1, 2}){
            color[i] = tex[i] * (diff + 0.6 * spec + amb) * (0.3f + visibility);
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
    Matrix4f getViewportMatrix(int width, int height);
    Matrix4f getProjectionMatrix(float eye_fov, float aspect_ratio, float zNear, float zFar);
    Matrix4f getOrthoProjectionMatrix(float l, float r, float b, float t, float n, float f);
    void rasterize_triangle(Vector4f a, Vector4f b, Vector4f c, IShader& shader, TGAImage& framebuffer, std::vector<float>& depthbuffer);
    void write_tga_file(std::string filename);
};



