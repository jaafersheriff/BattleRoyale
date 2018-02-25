#pragma once
#ifndef _FILE_READER_HPP_
#define _FILE_READER_HPP_
#include <fstream>

#include "ThirdParty/rapidjson/document.h"
#include "ThirdParty/rapidjson/filereadstream.h"
#include "EngineApp/EngineApp.hpp"

class FileReader {
public:
    static int loadLevel(const char & file);
    static void addSpatialComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform);
    static void addRenderComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform, const String filePath);
    static int addCapsuleColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    static int addSphereColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    static int addBoxColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
};

#endif