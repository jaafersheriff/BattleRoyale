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
    void initGameObject(String filePath, String texturePath, glm::vec3 position, glm::vec3 scale, glm::mat3 rotation);
};

#endif