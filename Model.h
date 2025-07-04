#pragma once

#include<vector>
#include<string>
#include "tgaimage.h"
#include "geometry.h"

class Model{
    public:
    void loadObject(std::string filename);
    TGAColor diffuse(Vector2f uv);
    TGAColor specular(Vector2f uv);
    TGAColor ao(Vector2f uv);
    Vector3f normal(Vector2f uv); 
    Vector3f getVertex(int face_index, int vert_index);
    Vector3f getNormal(int face_index, int vert_index);
    Vector2f getuv(int face_index, int vert_index);

    std::vector<Vector3i> vertex_inds;
    std::vector<Vector3i> norm_inds;
    std::vector<Vector3i> uv_inds;
    std::vector<Vector3f> vert;
    std::vector<Vector3f> norm;
    std::vector<Vector2f> uv;
    TGAImage diffuse_img;
    TGAImage specular_img;
    TGAImage normal_img;
    TGAImage ao_img;



    // std::vector<Triangle> triangles;
};