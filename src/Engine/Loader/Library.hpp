#pragma once
#ifndef _LIBRARY_HPP_
#define _LIBRARY_HPP_

#include "Model/Mesh.hpp"
#include "Model/Texture.hpp"

#include <unordered_map>

class Library {
    public:
        Mesh* getMesh(std::string name) {
            auto it = meshes.find(name);
            if (it != meshes.end()) {
                return it->second;
            }
            return nullptr;
        }
        void addMesh(std::string name, Mesh *mesh) {
            meshes.insert(std::unordered_map<std::string, Mesh *>::value_type(name, mesh));
        }

        Texture* getTexture(std::string name) {
            auto it = textures.find(name);
            if (it != textures.end()) {
                return it->second;
            }
            return nullptr;
        }
        void addTexture(std::string name, Texture *texture) {
            textures.insert(std::unordered_map<std::string, Texture*>::value_type(name, texture));
        }

    private:
        std::unordered_map<std::string, Mesh *> meshes;
        std::unordered_map<std::string, Texture *> textures;
};

#endif
