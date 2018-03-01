#pragma once
#ifndef _LIBRARY_HPP_
#define _LIBRARY_HPP_

#include "Model/Mesh.hpp"
#include "Model/Texture.hpp"

class Library {
    public:
        static UnorderedMap<String, Mesh *> meshes;
        static UnorderedMap<String, Texture *> textures;

        static Mesh* getMesh(const String & name) {
            auto it = Library::meshes.find(name);
            if (it != meshes.end()) {
                return it->second;
            }
            return nullptr;
        }
        static void addMesh(const String & name, Mesh & mesh) {
            meshes.insert(std::make_pair(name, &mesh));
        }

        static Texture* getTexture(const String & name) {
            auto it = textures.find(name);
            if (it != textures.end()) {
                return it->second;
            }
            return nullptr;
        }
        static void addTexture(const String & name, Texture *texture) {
            textures.insert(std::make_pair(name, texture));
        }
};

#endif
