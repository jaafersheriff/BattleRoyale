#pragma once
#ifndef _LIBRARY_HPP_
#define _LIBRARY_HPP_

#include "Model/Mesh.hpp"
#include "Model/Texture.hpp"

#include <string>
#include <unordered_map>
class Library {
    public:
        static UnorderedMap<std::string, Mesh *> meshes;
        static UnorderedMap<std::string, Texture *> textures;

        static Mesh* getMesh(const std::string & name) {
            auto it = Library::meshes.find(name);
            if (it != meshes.end()) {
                return it->second;
            }
            return nullptr;
        }
        static void addMesh(const std::string & name, Mesh & mesh) {
            meshes.insert({ name, &mesh });
        }

        static Texture* getTexture(const std::string & name) {
            auto it = textures.find(name);
            if (it != textures.end()) {
                return it->second;
            }
            return nullptr;
        }
        static void addTexture(const std::string & name, Texture *texture) {
            textures.insert({ name, texture });
        }
};

#endif
