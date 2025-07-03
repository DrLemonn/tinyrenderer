#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "Model.h"
#include <limits>
#include "rasterizer.h"

constexpr int width  = 800;
constexpr int height = 800;

const float PI = std::acos(-1.0);

std::vector<float> shadowDepthBuffer;

int main(int argc, char** argv) {
	rasterizer r(width, height);
	r.clear();

	Model m;

	if (2==argc) {
        m.loadObject(argv[1]);
    } else {
        m.loadObject("obj/diablo3_pose.obj");
    }

	Vector3f eye_pos{1.f, 1.0f, 4.f}, centre{0.f, 0.f, 0.f}, up{0.f, 1.f, 0.f};
    Vector3f light_dir{1.f, 1.f, 0.f};
    Matrix4f lightMVPMatrix;

    // shadow mapping first pass
    {
        TGAImage shadowMap{width, height, TGAImage::RGB};
        ShadowShader shader;

        shadowDepthBuffer.resize(height * width);
        std::fill(shadowDepthBuffer.begin(), shadowDepthBuffer.end(), std::numeric_limits<float>::max());
        
        shader.m = &m;
        shader.modelViewMatrix = r.lookAt(light_dir, centre, up) * r.getModelMatrix(0);
        shader.orthoProjectionMatrix = r.getOrthoProjectionMatrix(-1.5f,1.5f,-1.5f,1.5f,-0.1f,-10.f);
        shader.viewportMatrix = r.getViewportMatrix();

        lightMVPMatrix = shader.viewportMatrix * shader.orthoProjectionMatrix * shader.modelViewMatrix;

        for(int face_index = 0;face_index < m.vertex_inds.size();face_index++){
            Vector4f a,b,c;
            a = shader.vertex(face_index, 0);
            b = shader.vertex(face_index, 1);
            c = shader.vertex(face_index, 2);

            r.rasterize_triangle(a,b,c, shader, shadowMap, shadowDepthBuffer);
        }

        shadowMap.write_tga_file("shadowMap.tga");
    }

    


    {
        GouraudShader shader;

        Matrix4f modelView = r.lookAt(eye_pos, centre, up) * r.getModelMatrix(0);

        shader.m = &m;
        shader.modelViewMatrix = modelView;
        shader.modelViewInverTrans = modelView.inverse().transpose();
        shader.projectionMatrix= r.getProjectionMatrix(45.f, 1.f, -0.1f, -50.f);
        shader.viewportMatrix = r.getViewportMatrix();
        shader.lightMVPMatrix = lightMVPMatrix;
        shader.lightDir = light_dir;
        shader.lightDir.normalize();
        shader.eyePos = eye_pos;

        for(int face_index = 0;face_index < m.vertex_inds.size();face_index++){
            Vector4f a,b,c;
            a = shader.vertex(face_index, 0);
            b = shader.vertex(face_index, 1);
            c = shader.vertex(face_index, 2);

            r.rasterize_triangle(a,b,c, shader, r.framebuffer, r.depthbuffer);
        }

        // 保存图像
	    r.write_tga_file("wired_frame.tga");
    }



    return 0;
}