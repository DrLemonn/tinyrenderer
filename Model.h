#pragma once

#include<vector>
#include<string>
#include "geometry.h"

class Model{
    public:
    bool loadObject(std::string filename);

    //private:
    std::vector<Vector3i> vertex_inds;
    std::vector<Vector3f> vert;
    //std::vector<Triangle> triangles; 
};