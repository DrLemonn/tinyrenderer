#include "Model.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

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

        // if(tokens.size() != 4){
        //     std::cerr << "not valid obj file format" <<std::endl;
        //     std::cerr << "the tokens are :";
        //     for(const auto& token : tokens){
        //         std::cerr<<token<<" ";
        //     }
        //     std::cerr<<std::endl;
        //     return false;   
        // }  

        if(tokens[0].compare("v") == 0){
            std::vector<float> vertex;

            for(int i = 1;i < 4;i++){
                vertex.push_back(std::stof(tokens[i])); 
            }

            vert.push_back(std::move(vertex));
        } else if(tokens[0].compare("f") == 0){
            std::vector<int> temp_ind;
            for(int i = 1;i < 4;i++){
                std::vector<std::string> ts = split(tokens[i], "/");
                temp_ind.push_back(std::stoi(ts[0]));
            }

            ind.push_back(std::move(temp_ind));
        }

    }
}


