#pragma once
#ifndef _FILE_READER_HPP_
#define _FILE_READER_HPP_
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include "ThirdParty/rapidjson/document.h"
#include "ThirdParty/rapidjson/filereadstream.h"
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
    int loadLevel(const char & file);
    void addSpatialComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform);
    void addRenderComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform, const std::string filePath);
    int addCapsuleColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    int addSphereColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    int addBoxColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
};

#endif