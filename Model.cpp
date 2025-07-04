#include "Model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "utils.h"

void Model::loadObject(std::string filename){
    std::ifstream inputFile(filename);

    if(!inputFile.is_open()){
        std::cerr << "Can't open file" <<std::endl;
    }

    std::string s;

    while(std::getline(inputFile, s)){
        std::istringstream iss(s);
        std::string token;
        std::vector<std::string> tokens;

        if(s.size() == 0){
            continue;
        }

        while(iss >> token){
            tokens.push_back(token);
        }

        if(tokens[0].compare("v") == 0){
            vert.push_back(Vector3f(std::stof(tokens[1]),
                                    std::stof(tokens[2]),
                                    std::stof(tokens[3])));
        } else if (tokens[0].compare("vt") == 0) { 
            uv.push_back(Vector2f(std::stof(tokens[1]),
                                  std::stof(tokens[2])));
        } else if (tokens[0].compare("vn") == 0) {
            norm.push_back(Vector3f(std::stof(tokens[1]),
                                    std::stof(tokens[2]),
                                    std::stof(tokens[3])));
        } else if (tokens[0].compare("f") == 0){
            Vector3i vertex_ind;
            Vector3i uv_ind;
            Vector3i norm_ind;
            for(int i = 1;i < 4;i++){
                std::vector<std::string> ts = split(tokens[i], "/");
                vertex_ind[i-1] = std::stoi(ts[0]) - 1;
                uv_ind[i-1] = std::stoi(ts[1]) - 1;
                norm_ind[i-1] = std::stoi(ts[2]) - 1;
            }
            vertex_inds.push_back(vertex_ind);
            norm_inds.push_back(norm_ind);
            uv_inds.push_back(uv_ind);
        }

    }

    std::cerr << "# v# " << vert.size() << " f# "  << vertex_inds.size() << " vt# " << uv.size() << " vn# " << norm.size() << std::endl;


    auto load_texture = [&filename](const std::string suffix, TGAImage &img) {
        size_t dot = filename.find_last_of(".");
        if (dot==std::string::npos) return;
        std::string texfile = filename.substr(0,dot) + suffix;
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    };
    load_texture("_diffuse.tga",    diffuse_img );
    load_texture("_nm_tangent.tga", normal_img  );
    //load_texture("_nm.tga", normal_img  );
    load_texture("_spec.tga",       specular_img);
    load_texture("_ao.tga",       ao_img);
}

TGAColor Model::diffuse(Vector2f uv)
{
    return diffuse_img.get(std::floor(uv.x * diffuse_img.width()), std::floor(uv.y * diffuse_img.height()));
}

TGAColor Model::specular(Vector2f uv)
{
    return specular_img.get(std::floor(uv.x * specular_img.width()), std::floor(uv.y * specular_img.height()));
}

TGAColor Model::ao(Vector2f uv)
{
    return ao_img.get(std::floor(uv.x * ao_img.width()), std::floor(uv.y * ao_img.height()));
}

Vector3f Model::normal(Vector2f uv)
{      
    Vector3f result;
    TGAColor n = normal_img.get(std::floor(uv.x * normal_img.width()), std::floor(uv.y * normal_img.height()));

    for(int i = 0;i < 3;i++){
        result[2 - i] =  static_cast<float>(n[i])/255.f * 2.f - 1.f;
    }
    
    return result;
}

Vector3f Model::getVertex(int face_index, int vert_index){
    return vert[vertex_inds[face_index][vert_index]];
}

Vector3f Model::getNormal(int face_index, int vert_index)
{
    return norm[norm_inds[face_index][vert_index]];
}

Vector2f Model::getuv(int face_index, int vert_index)
{
    return uv[uv_inds[face_index][vert_index]];
}
