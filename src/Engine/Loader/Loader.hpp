/* Loader class
 * Responsible for loading in external files
 * Responsible for loading CPU memory onto GPU */
#pragma once
#ifndef _LOADER_HPP_
#define _LOADER_HPP_

#include "Library.hpp"

#include <cstdint>

class Mesh;
class Texture;
class Loader {
    public:
        void init(bool, const std::string &);

        /* Retrieve Mesh pointer from an .obj file*/
        Mesh *getMesh(const std::string &);

        /* Retrieve GL Texture ID */
        Texture *getTexture(const std::string &);
        Texture *getTexture(const std::string &, GLenum, const bool);

    private:
        /* Resize mesh vertex buffers so all the vertices are [0, 1] */
        void resize(Mesh::MeshBuffers &);

        /* Return pointer to loaded stb_image data */
        uint8_t* loadTextureData(const std::string &, const bool, int*, int*, int*);

        /* GL Loaders */
        void loadMesh(Mesh &);
        void loadTexture(Texture *, uint8_t *, GLenum);

        /* Private members */
        Library library;
        std::string RESOURCE_DIR;
        bool verbose;
};

#endif