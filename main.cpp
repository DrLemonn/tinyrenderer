#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "Model.h"
#include <limits>
#include "rasterizer.h"
#include <random>

constexpr int width  = 1024;
constexpr int height = 1024;

const float PI = std::acos(-1.0);

std::vector<float> shadowDepthBuffer;
TGAImage total{1024,1024,TGAImage::GRAYSCALE};
TGAImage occl{1024,1024, TGAImage::GRAYSCALE};

Vector3f randomPointOnSphere(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> distrib(0.0f, 1.0f);
    float u,v;

    u = distrib(gen);
    v = distrib(gen);

    float theta, phi;

    theta = 2 * u * PI;
    phi = std::acos(2 * v - 1);

    Vector3f p;
    p.x = std::sin(phi) * std::cos(theta);
    p.y = std::sin(phi) * std::sin(theta);
    p.z = std::cos(phi);

    return p;
}

struct AOShader : IShader{
    // --- "Uniforms" ---
    Model* m = nullptr;
    Matrix4f modelViewMatrix;
    Matrix4f projectionMatrix;
    Matrix4f viewportMatrix;

    // --- "Varying" ---
    Matrix<4, 3, float> varing_fragPosition;
    Matrix<2, 3, float> varing_uv;

    Vector4f vertex(int face_index, int vert_index) override {
        Matrix4f acc = viewportMatrix * projectionMatrix * modelViewMatrix;
        Vector3f v_pos = m->getVertex(face_index, vert_index);
        varing_fragPosition.set_col(vert_index, acc * toVec4(v_pos));
        varing_uv.set_col(vert_index, m->getuv(face_index, vert_index)); 
        return acc * toVec4(v_pos);
    }

    bool fragment(Vector3f bar, TGAColor& color) override {
        Vector4f p = varing_fragPosition * bar;
        Vector2f uv = varing_uv * bar;
        
        // use shadow buffer
        if(std::abs(p.z - shadowDepthBuffer[int(p.y) * width + int(p.x)]) < 1e-3){
            occl.set(uv.x * 1024, uv.y * 1024, TGAColor(255));
        }

        color = TGAColor{255,0,0};
        return false; 
    }
};

struct UVShader : IShader{
    // --- "Uniforms" ---
    Model* m = nullptr;
    Matrix4f modelViewMatrix;
    Matrix4f projectionMatrix;
    Matrix4f viewportMatrix;

    // --- "Varying" ---
    Matrix<2, 3, float> varing_uv;

    Vector4f vertex(int face_index, int vert_index) override {
        Matrix4f acc = viewportMatrix * projectionMatrix * modelViewMatrix;
        Vector3f v_pos = m->getVertex(face_index, vert_index);
        varing_uv.set_col(vert_index, m->getuv(face_index, vert_index)); 
        return acc * toVec4(v_pos);
    }

    bool fragment(Vector3f bar, TGAColor& color) override {
        Vector2f uv = varing_uv * bar;
        
        unsigned char g = total.get(uv.x * 1024, uv.y * 1024)[0];

        //unsigned char g = m->ao(uv)[0];
        
        color = TGAColor{g,g,g};
        return false; 
    }
};

int main(int argc, char** argv) {
	rasterizer r(width, height);
	r.clear();

	Model m;

	if (2==argc) {
        m.loadObject(argv[1]);
    } else {
        m.loadObject("obj/diablo3_pose.obj");
    }

    // ambient occlusion
    // shadow mapping first pass

    int nSample = 500;
    shadowDepthBuffer.resize(height * width);

    for(int i = 0;i < nSample;i++){
        Vector3f eye_pos = randomPointOnSphere(), centre{}, up{};
        // 随机生成一个up向量，防止与glaze方向太过接近导致叉乘结果接近0出现误差
        for (int i=0; i<3; i++) up[i] = (float)rand()/(float)RAND_MAX;

        TGAImage shadowMap{width, height, TGAImage::RGB};
        ShadowShader ShadowShader;
        
        std::fill(shadowDepthBuffer.begin(), shadowDepthBuffer.end(), std::numeric_limits<float>::max());
        occl.clear();

        ShadowShader.m = &m;
        ShadowShader.modelViewMatrix = r.lookAt(eye_pos, centre, up) * r.getModelMatrix(0);
        ShadowShader.orthoProjectionMatrix = r.getOrthoProjectionMatrix(-2.f,2.f,-2.f,2.f,-0.1f,-10.f);
        ShadowShader.viewportMatrix = r.getViewportMatrix(1024, 1024);

        for(int face_index = 0;face_index < m.vertex_inds.size();face_index++){
            Vector4f a,b,c;
            a = ShadowShader.vertex(face_index, 0);
            b = ShadowShader.vertex(face_index, 1);
            c = ShadowShader.vertex(face_index, 2);

            r.rasterize_triangle(a,b,c, ShadowShader, shadowMap, shadowDepthBuffer);
        }

        // shadowMap.write_tga_file("shadowMap.tga");

        AOShader shader;

        r.clear();

        shader.m = &m;
        shader.modelViewMatrix = ShadowShader.modelViewMatrix;
        shader.projectionMatrix= ShadowShader.orthoProjectionMatrix;
        shader.viewportMatrix = ShadowShader.viewportMatrix;

        for(int face_index = 0;face_index < m.vertex_inds.size();face_index++){
            Vector4f a,b,c;
            a = shader.vertex(face_index, 0);
            b = shader.vertex(face_index, 1);
            c = shader.vertex(face_index, 2);

            r.rasterize_triangle(a,b,c, shader, r.framebuffer, r.depthbuffer);
        }

        for(int x = 0;x < 1024;x++){
            for(int y = 0;y < 1024;y++){
                float total_g = total.get(x, y)[0];
                float occl_g = occl.get(x, y)[0];
                total.set(x, y, TGAColor((total_g * i + occl_g)/float(i + 1) + 0.5f));
            }
        }
        
    }

    {
        r.clear();

        UVShader uvshader;
        uvshader.m = &m;
        
        uvshader.modelViewMatrix = r.lookAt({1.f,1.f,4.f}, {0.f,0.f,0.f}, {0.f,1.f,0.f}) * r.getModelMatrix(0);;
        uvshader.projectionMatrix= r.getProjectionMatrix(45.f, 1.f, -0.1f, -50.f);
        uvshader.viewportMatrix = r.getViewportMatrix(width, height);

        for(int face_index = 0;face_index < m.vertex_inds.size();face_index++){
            Vector4f a,b,c;
            a = uvshader.vertex(face_index, 0);
            b = uvshader.vertex(face_index, 1);
            c = uvshader.vertex(face_index, 2);

            r.rasterize_triangle(a,b,c, uvshader, r.framebuffer, r.depthbuffer);
        }
    }

    occl.write_tga_file("occl.tga");
    total.write_tga_file("total.tga");
    r.write_tga_file("diablo.tga");

    return 0;
}