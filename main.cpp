#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "Model.h"
#include <limits>
#include "rasterizer.h"

// 确保 tgaimage.h 中包含了必要的头文件和 TGAImage 类定义

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};
constexpr TGAColor black   = {  0,   0,   0, 255}; // 定义一个黑色，如果需要清零

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

	Vector3f eye_pos{0.f, 2.0f, 5.f}, centre{0.f, 0.f, 0.f}, up{0.f, 1.f, 0.f};

	for(const auto& ind : m.vertex_inds){

		Vector4f a = toVec4(m.vert[ind[0]]);
		Vector4f b = toVec4(m.vert[ind[1]]);
		Vector4f c = toVec4(m.vert[ind[2]]);
		
		Matrix4f model = r.getModelMatrix(0);
		Matrix4f view = r.lookAt(eye_pos, centre, up);
		Matrix4f projection = r.getProjectionMatrix(45.f, 1.f, -0.1f, -50.f);
		Matrix4f viewport = r.getViewportMatrix();

		Matrix4f mvp =  projection * view * model;

		// 别忘了处理齐次坐标
		a = mvp * a;
		b = mvp * b;
		c = mvp * c;
		a = a / a.w;
		b = b / b.w;
		c = c / c.w;
		a = viewport * a;
		b = viewport * b;
		c = viewport * c;

		TGAColor rnd;
        for (int ci=0; ci<3; ci++){
			rnd[ci] = std::rand()%255;
		} 
		
		r.rasterize_triangle(a,b,c,rnd);
	}

	//framebuffer.flip_vertically();
	// 保存图像
	r.write_tga_file("wired_frame.tga");

    return 0;
}