#pragma once
#ifndef _FILE_READER_HPP_
#define _FILE_READER_HPP_
#include <fstream>

#include "ThirdParty/rapidjson/document.h"
#include "ThirdParty/rapidjson/filereadstream.h"
#include "EngineApp/EngineApp.hpp"

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