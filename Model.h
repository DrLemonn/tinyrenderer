#pragma once

#include<vector>
#include<string>
#include "tgaimage.h"
#include "geometry.h"

class Model{
    public:
    bool loadObject(std::string filename);
    void loadTexture(std::string filename, std::string suffix, TGAImage texture);
    TGAColor diffuse(Vector2f uv);
    TGAColor specular(Vector2f uv);
    Vector3f normal(Vector2f uv); 

    std::vector<Vector3i> vertex_inds;
    std::vector<Vector3i> norm_inds;
    std::vector<Vector3i> uv_inds;
    std::vector<Vector3f> vert;
    std::vector<Vector3f> norm;
    std::vector<Vector2f> uv;
    TGAImage diffuse_img;
    TGAImage specular_img;
    TGAImage normal_img;
    //std::vector<Triangle> triangles; 
};