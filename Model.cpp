#include "Model.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "utils.h"

bool Model::loadObject(std::string filename){
    std::ifstream inputFile(filename);

    if(!inputFile.is_open()){
        std::cerr << "Can't open file" <<std::endl;
        return false;
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
        } else if(tokens[0].compare("f") == 0){
            Vector3i vertex_ind;
            for(int i = 1;i < 4;i++){
                std::vector<std::string> ts = split(tokens[i], "/");
                vertex_ind[i-1] = std::stoi(ts[0]) - 1;
            }
            vertex_inds.push_back(vertex_ind);
        }

    }
}


