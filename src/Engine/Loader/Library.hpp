#pragma once
#ifndef _LIBRARY_HPP_
#define _LIBRARY_HPP_

#include "Model/Mesh.hpp"
#include "Model/Texture.hpp"

#include <string>
#include <unordered_map>

class Library {
    public:
        Mesh* getMesh(const std::string & name) {
            auto it = meshes.find(name);
            if (it != meshes.end()) {
                return it->second;
            }
            return nullptr;
        }
        void addMesh(const std::string & name, Mesh & mesh) {
            meshes.insert({ name, &mesh });
        }

        Texture* getTexture(const std::string & name) {
            auto it = textures.find(name);
            if (it != textures.end()) {
                return it->second;
            }
            return nullptr;
        }
        void addTexture(const std::string & name, Texture *texture) {
            textures.insert({ name, texture });
        }

    private:
        std::unordered_map<std::string, Mesh *> meshes;
        std::unordered_map<std::string, Texture *> textures;
};

#endif
