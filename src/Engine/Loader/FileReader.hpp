#pragma once
#ifndef _FILE_READER_HPP_
#define _FILE_READER_HPP_
#include <fstream>

#include "ThirdParty/rapidjson/document.h"
#include "ThirdParty/rapidjson/filereadstream.h"
#include "EngineApp/EngineApp.hpp"

class FileReader {
public:
    static int loadLevel(const char & file, float ambience);
    static SpatialComponent & addSpatialComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform);
    static DiffuseRenderComponent & addRenderComponent(GameObject & gameObject, const SpatialComponent & spatial, const rapidjson::Value& jsonTransform, const String filePath, float ambience);
    static int addCapsuleColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    static int addSphereColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
    static int addBoxColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject);
};

#endif