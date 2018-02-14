#pragma once
#ifndef _FILE_READER_HPP_
#define _FILE_READER_HPP_
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "EngineApp/EngineApp.hpp"

inline bool readTextFile(const std::string & filepath, std::string & dst) {
    std::ifstream ifs(filepath);
    if (!ifs.good()) {
        return false;
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();
    dst = ss.str();

    return true;
}

class FileReader {
public:
    int loadLevel(const char & file, Scene & scene);
    void initGameObject(Scene & scene, std::string filePath, std::string texturePath, glm::vec3 position, glm::vec3 scale, glm::mat3 rotation);
};

#endif