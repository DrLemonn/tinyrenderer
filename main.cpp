#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "Model.h"
#include <limits>

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



float computeSignedArea(Vector2f v1, Vector2f v2){
	return 0.5f * (v1.x * v2.y - v2.x * v1.y);
}

// compute Barycentric on 2d space
std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f& a, const Vector3f& b, const Vector3f& c){
	
	// BCXBP / BCXBA
	float alpha = computeSignedArea(Vector2f{c.x - b.x,c.y - b.y}, Vector2f{x - b.x, y - b.y})/ 
		computeSignedArea(Vector2f{c.x - b.x,c.y - b.y}, Vector2f{a.x - b.x, a.y - b.y});
	// CAXCP / CAXCB
	float beta = computeSignedArea(Vector2f{a.x - c.x,a.y - c.y}, Vector2f{x - c.x, y - c.y})/ 
		computeSignedArea(Vector2f{a.x - c.x,a.y - c.y}, Vector2f{b.x - c.x, b.y - c.y});

	float gamma = computeSignedArea(Vector2f{b.x - a.x,b.y - a.y}, Vector2f{x - a.x, y - a.y})/ 
		computeSignedArea(Vector2f{b.x - a.x,b.y - a.y}, Vector2f{c.x - a.x, c.y - a.y});
	return {alpha, beta, gamma};
}


int getIndex(int x, int y){
	return y*width + x;
}

bool insideTriangle(float x, float y, Vector3f a, Vector3f b, Vector3f c){
	a[2]=b[2]=c[2]=1.0f;
	Vector3f p(x,y,1.0f);

	// check if crossproduct have same sign
	Vector3f ab_ap_cross = cross(b - a, p - a);
	Vector3f bc_bp_cross = cross(c - b, p - b);
	Vector3f ca_cp_cross = cross(a - c, p - c);

	bool has_neg = (ab_ap_cross.z < 0 || bc_bp_cross.z < 0 || ca_cp_cross.z < 0);
	bool has_pos = (ab_ap_cross.z > 0 || bc_bp_cross.z > 0 || ca_cp_cross.z > 0);

	return !(has_neg && has_pos);
}

Matrix4f getModelMatrix(float angle){
	Matrix4f model;
	model[0] = {std::cos((angle/180.f)*PI), 0.f, std::sin((angle/180.f)*PI), 0.f};
	model[1] = {0.f, 1.f, 0.f, 0.f};
	model[2] = {-std::sin((angle/180.f)*PI), 0.f, std::cos((angle/180.f)*PI), 0.f};
	model[3] = {0.f, 0.f, 0.f, 1.f};

	return model;
}


// 注意叉乘的方向和顺序，如果弄反了图像会出现翻转的现象，一定要每一个轴和xyz对应上
Matrix4f lookAt(Vector3f eye_pos, Vector3f centre, Vector3f up){
	Vector3f g = centre - eye_pos;
	g.normalize();
	Vector3f gcrossu = cross(g, up);
	gcrossu.normalize();
	Vector3f u = cross(gcrossu, g);
	u.normalize();

	Matrix4f trans, rot;

	trans[0] = {1.f, 0.f, 0.f, -eye_pos.x};
	trans[1] = {0.f, 1.f, 0.f, -eye_pos.y};
	trans[2] = {0.f, 0.f, 1.f, -eye_pos.z};
	trans[3] = {0.f, 0.f, 0.f, 1.f};

	rot[0] = toVec4(gcrossu, 0.f);
	rot[1] = toVec4(u, 0.f);
	rot[2] = toVec4(-g, 0.f);
	rot[3] = {0.f, 0.f, 0.f, 1.f};

	return rot * trans;
}

Matrix4f getProjectionMatrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    Matrix4f projection = Matrix4f::identity();
    
    Matrix4f orthographic, perspective;
    float b,t,l,r;

	perspective[0] = {zNear, 0.0f, 0.0f, 0.0f};
	perspective[1] = {0.0f, zNear, 0.0f, 0.0f};
	perspective[2] = {0.0f, 0.0f, zNear + zFar, -zNear * zFar};
	perspective[3] = {0.0f, 0.0f, 1.0f, 0.0f};
    t = std::atan(eye_fov/2.0/180.0*PI) * std::abs(zNear);
    r = t * aspect_ratio;
    l = -r;
    b = -t;


	orthographic[0] = {2/(r-l), 0.0f, 0.0f, 0.0f};
	orthographic[1] = {0.0f, 2/(t-b), 0.0f, 0.0f};
	orthographic[2] = {0.0f, 0.0f, 2/(zNear-zFar), -(zNear + zFar)/(zNear - zFar)};
	orthographic[3] = {0.0f, 0.0f, 0.0f, 1.0f};
	

    projection = orthographic * perspective * projection;

    return projection;
}


Matrix4f getViewportMatrix(){
	Matrix4f viewport;
	viewport[0] = {width/2.0f, 0.f, 0.f, width/2.0f};
	viewport[1] = {0.f, height/2.0f, 0.f, height/2.0f};
	viewport[2] = {0.f, 0.f, -0.5f, 0.5f};
	viewport[3] = {0.f, 0.f, 0.f, 1.f};

	return viewport;
}

void rasterize_triangle(Vector4f a, Vector4f b, Vector4f c, std::vector<float>& depthbuffer, TGAImage& framebuffer, TGAColor color){
	// compute bounding box
	int bbminx = std::floor(std::min(a.x, std::min(b.x, c.x)));
	int bbmaxx = std::floor(std::max(a.x, std::max(b.x, c.x)));
	int bbminy = std::floor(std::min(a.y, std::min(b.y, c.y)));
	int bbmaxy = std::floor(std::max(a.y, std::max(b.y, c.y)));

	for(int j = bbminy;j <= bbmaxy;j++){
		for(int i = bbminx; i <= bbmaxx;i++){
			float x = i + 0.5f;
			float y = j + 0.5f;
			if(insideTriangle(x, y, toVec3(a), toVec3(b), toVec3(c))){
				int index = getIndex(i,j);
				
				auto [alpha, beta, gamma] = computeBarycentric2D(x, y, toVec3(a), toVec3(b), toVec3(c));
				float z_interpolated = alpha * a.z + beta * b.z + gamma * c.z;

				if(z_interpolated < depthbuffer[index]){
					depthbuffer[index] = z_interpolated;
					framebuffer.set(i, j, color);
				}
			}
		}
	}
}



int main(int argc, char** argv) {

    TGAImage framebuffer(width, height, TGAImage::RGB);
	std::vector<float> depthbuffer(width * height);
	fill(depthbuffer.begin(), depthbuffer.end(), std::numeric_limits<float>::max());

	Model m;

	m.loadObject("diablo3_pose.obj");

	// std::cout<<m.vertex_inds.size()<<std::endl;
	// std::cout<<m.vert.size()<<std::endl;

	Vector3f eye_pos{0.f, 0.0f, 3.f}, centre{0.f, 0.f, 0.f}, up{0.f, 1.f, 0.f};

	for(const auto& ind : m.vertex_inds){
		//std::cout<<"Triangle with vertex index:"<<ind[0]<<" "<<ind[1]<<" "<<ind[2]<<" "<<std::endl;

		// for(int i = 0;i < 3;i++){

		// 	//obj f的index要减1
		// 	Vector3f v1 = m.vert[ind[i]];
		// 	Vector3f v2 = m.vert[ind[(i+1)%3]];
		// 	float ax,ay,bx,by;

		// 	ax = std::round((v1[0]*0.5 + 0.5)*1024);
		// 	ay = std::round((v1[1]*0.5 + 0.5)*1024);
		// 	bx = std::round((v2[0]*0.5 + 0.5)*1024);
		// 	by = std::round((v2[1]*0.5 + 0.5)*1024);
		// 	//std::cout<<ax<<" "<<ay<<" "<<bx<<" "<<by<<std::endl;
		// 	line(ax, ay, bx, by, framebuffer, red);
		// }

		Vector4f a = toVec4(m.vert[ind[0]]);
		Vector4f b = toVec4(m.vert[ind[1]]);
		Vector4f c = toVec4(m.vert[ind[2]]);
		
		Matrix4f model = getModelMatrix(0);
		Matrix4f view = lookAt(eye_pos, centre, up);
		Matrix4f projection = getProjectionMatrix(45.f, 1.f, -0.1f, -50.f);
		Matrix4f viewport = getViewportMatrix();

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
		
		rasterize_triangle(a,b,c,depthbuffer,framebuffer,rnd);
	}

	//framebuffer.flip_vertically();
	framebuffer.write_tga_file("wired_frame.tga"); // 保存图像


    // framebuffer.flip_vertically(); // 翻转图像
    // framebuffer.write_tga_file("Flipped.tga"); // 保存图像
    return 0;
}