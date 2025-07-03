#include "rasterizer.h"
#include "utils.h"


void rasterizer::clear(){
    fill(depthbuffer.begin(), depthbuffer.end(), std::numeric_limits<float>::max());
    framebuffer.clear();
}

void rasterizer::line(int ax, int ay, int bx, int by, TGAColor color) {
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

Matrix4f rasterizer::getModelMatrix(float angle)
{
    Matrix4f model;
	model[0] = {std::cos((angle/180.f)*PI), 0.f, std::sin((angle/180.f)*PI), 0.f};
	model[1] = {0.f, 1.f, 0.f, 0.f};
	model[2] = {-std::sin((angle/180.f)*PI), 0.f, std::cos((angle/180.f)*PI), 0.f};
	model[3] = {0.f, 0.f, 0.f, 1.f};

	return model;
}

// 注意叉乘的方向和顺序，如果弄反了图像会出现翻转的现象，一定要每一个轴和xyz对应上
Matrix4f rasterizer::lookAt(Vector3f eye_pos, Vector3f centre, Vector3f up){
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

Matrix4f rasterizer::getProjectionMatrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
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

Matrix4f rasterizer::getOrthoProjectionMatrix(float l, float r, float b, float t, float n, float f)
{
	Matrix4f orthographic;

	orthographic[0] = {2/(r-l), 0.0f, 0.0f, 0.0f};
	orthographic[1] = {0.0f, 2/(t-b), 0.0f, 0.0f};
	orthographic[2] = {0.0f, 0.0f, 2/(n-f), -(n + f)/(n - f)};
	orthographic[3] = {0.0f, 0.0f, 0.0f, 1.0f};
    return orthographic;
}

Matrix4f rasterizer::getViewportMatrix(){
	Matrix4f viewport;
	viewport[0] = {width/2.0f, 0.f, 0.f, width/2.0f};
	viewport[1] = {0.f, height/2.0f, 0.f, height/2.0f};
	viewport[2] = {0.f, 0.f, -0.5f, 0.5f};
	viewport[3] = {0.f, 0.f, 0.f, 1.f};

	return viewport;
}

void rasterizer::rasterize_triangle(Vector4f a, Vector4f b, Vector4f c, IShader& shader, TGAImage& framebuffer, std::vector<float>& depthbuffer){
	// 传过来的abc还没有做除以w，利用w的值来做透视矫正插值
    Vector3f w{a.w, b.w, c.w};
    a = a/w[0];
    b = b/w[1];
    c = c/w[2];
    
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
				
                // 详见barycentric coordinate笔记
				auto [alpha_prime, beta_prime, gamma_prime] = computeBarycentric2D(x, y, toVec3(a), toVec3(b), toVec3(c));
				float k = 1.f/(alpha_prime/w[0] + beta_prime/w[1] + gamma_prime/w[2]);
                float alpha = k * alpha_prime / w[0];
                float beta = k * beta_prime / w[1];
                float gamma = k * gamma_prime / w[2];
                
                float z_interpolated = alpha * a.z + beta * b.z + gamma * c.z;

				if(z_interpolated < depthbuffer[index]){
                    TGAColor color{};
                    bool ignore = shader.fragment({alpha, beta, gamma}, color);                

                    if(!ignore){
                        depthbuffer[index] = z_interpolated;
					    framebuffer.set(i, j, color);     
                    }

				}
			}
		}
	}
}

void rasterizer::write_tga_file(std::string filename)
{
    //framebuffer.flip_vertically();
    framebuffer.write_tga_file(filename.c_str()); 
}
