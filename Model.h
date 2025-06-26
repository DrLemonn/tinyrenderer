#pragma once

#include<vector>
#include<string>

class Model{
    public:
    bool loadObject(std::string filename);

    //private:
    std::vector<std::vector<int>> ind;
    std::vector<std::vector<float>> vert;
};