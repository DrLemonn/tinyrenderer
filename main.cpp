#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "Model.h"

// 确保 tgaimage.h 中包含了必要的头文件和 TGAImage 类定义

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};
constexpr TGAColor black   = {  0,   0,   0, 255}; // 定义一个黑色，如果需要清零

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {

	
	bool steep = false;
	if(std::abs(ax - bx) < std::abs(ay - by)){
		std::swap(ax, ay);
		std::swap(bx, by);
		steep = true;
	}

	if(ax > bx){
		std::swap(ax, bx);
		std::swap(ay, by);
	}

	for(int x = ax;x <= bx;x++){
		float t = static_cast<float>(x-ax)/(bx - ax);
		int y = std::round(ay + (by - ay)*t);
		
		if(steep){
			framebuffer.set(y, x, color);
		} else {
			framebuffer.set(x, y, color);
		}
		
	}	
}



int main(int argc, char** argv) {
    constexpr int width  = 1024;
    constexpr int height = 1024;
    TGAImage framebuffer(width, height, TGAImage::RGB);

	Model m;

	m.loadObject("diablo3_pose.obj");

	std::cout<<m.vertex_inds.size()<<std::endl;
	std::cout<<m.vert.size()<<std::endl;

	for(const auto& ind : m.vertex_inds){
		//std::cout<<"Triangle with vertex index:"<<ind[0]<<" "<<ind[1]<<" "<<ind[2]<<" "<<std::endl;

		for(int i = 0;i < 3;i++){

			//obj f的index要减1
			Vector3f v1 = m.vert[ind[i]];
			Vector3f v2 = m.vert[ind[(i+1)%3]];
			float ax,ay,bx,by;

			ax = std::round((v1[0]*0.5 + 0.5)*1024);
			ay = std::round((v1[1]*0.5 + 0.5)*1024);
			bx = std::round((v2[0]*0.5 + 0.5)*1024);
			by = std::round((v2[1]*0.5 + 0.5)*1024);
			//std::cout<<ax<<" "<<ay<<" "<<bx<<" "<<by<<std::endl;
			line(ax, ay, bx, by, framebuffer, red);
		}
	}

	framebuffer.flip_vertically();
	framebuffer.write_tga_file("wired_frame.tga"); // 保存图像


    // framebuffer.flip_vertically(); // 翻转图像
    // framebuffer.write_tga_file("Flipped.tga"); // 保存图像
    return 0;
}