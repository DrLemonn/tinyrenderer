#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "Model.h"
#include <limits>
#include "rasterizer.h"

constexpr int width  = 800;
constexpr int height = 800;

const float PI = std::acos(-1.0);

int main(int argc, char** argv) {
	rasterizer r(width, height);
	r.clear();

	Model m;

	if (2==argc) {
        m.loadObject(argv[1]);
    } else {
        m.loadObject("obj/african_head.obj");
    }

	Vector3f eye_pos{1.f, 1.0f, 3.f}, centre{0.f, 0.f, 0.f}, up{0.f, 1.f, 0.f};

	GouraudShader shader;

	Matrix4f modelView = r.lookAt(eye_pos, centre, up) * r.getModelMatrix(0);

	shader.m = &m;
	shader.modelViewMatrix = modelView;
	shader.modelViewInverTrans = modelView.inverse().transpose();
	shader.projectionMatrix= r.getProjectionMatrix(45.f, 1.f, -0.1f, -50.f);
	shader.viewportMatrix = r.getViewportMatrix();
	shader.lightDir = Vector3f{1.f, 1.f, 1.f};
	shader.lightDir.normalize();
	shader.eyePos = eye_pos;

	for(int face_index = 0;face_index < m.vertex_inds.size();face_index++){
		Vector4f a,b,c;
		a = shader.vertex(face_index, 0);
		b = shader.vertex(face_index, 1);
		c = shader.vertex(face_index, 2);

		r.rasterize_triangle(a,b,c, shader);
	}
	// 保存图像
	r.write_tga_file("wired_frame.tga");

    return 0;
}