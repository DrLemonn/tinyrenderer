#pragma once

#include <vector>
#include <string>


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