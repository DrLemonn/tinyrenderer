#include "utils.h"

std::vector<std::string> split(std::string s,std::string delimiter){
    std::vector<std::string> result;
    size_t pos_start = 0, pos_end;

    while((pos_end = s.find(delimiter, pos_start)) != std::string::npos){
       std::string token = s.substr(pos_start, pos_end - pos_start);
       result.push_back(token);
       pos_start = pos_end + delimiter.length();
    }
    
    if(pos_start < s.length()){
        result.push_back(s.substr(pos_start, s.length() - pos_start));
    }

    return result;
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