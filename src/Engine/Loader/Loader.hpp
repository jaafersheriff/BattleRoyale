/* Loader class
 * Responsible for loading in external files
 * Responsible for loading CPU memory onto GPU */
#pragma once
#ifndef _LOADER_HPP_
#define _LOADER_HPP_

#define GLEW_STATIC
#include <GL/glew.h>
#include <cstdint>

#include "Model/Mesh.hpp"
#include "Library.hpp"

class Texture;
class Loader {
    public:
        static void init(bool, const std::string &);

        /* Retrieve Mesh pointer from an .obj file*/
        static Mesh* getMesh(const std::string &);

        /* Retrieve GL Texture ID */
        static Texture *getTexture(const std::string &);
        static Texture *getTexture(const std::string &, GLenum, const bool);

    private:
        /* Resize mesh vertex buffers so all the vertices are [0, 1] */
        static void resize(Mesh::MeshBuffers &);

        /* Return pointer to loaded stb_image data */
        static uint8_t* loadTextureData(const std::string &, const bool, int*, int*, int*);

        /* GL Loaders */
        static void loadMesh(Mesh &);
        static void loadTexture(Texture *, uint8_t *, GLenum);

        /* Private members */
        static std::string RESOURCE_DIR;
        static bool verbose;
};

#endif