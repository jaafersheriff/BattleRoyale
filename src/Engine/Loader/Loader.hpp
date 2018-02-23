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
        static void init(bool, const String &);

        /* Retrieve Mesh pointer from an .obj file*/
        static Mesh* getMesh(const String &);

        /* Retrieve GL Texture ID */
        static Texture *getTexture(const String &);
        static Texture *getTexture(const String &, GLenum, const bool);

    private:
        /* Resize mesh vertex buffers so all the vertices are [0, 1] */
        static void resize(Mesh::MeshBuffers &);

        /* Return pointer to loaded stb_image data */
        static uint8_t* loadTextureData(const String &, const bool, int*, int*, int*);

        /* GL Loaders */
        static void loadMesh(Mesh &);
        static void loadTexture(Texture *, uint8_t *, GLenum);

        /* Private members */
        static String RESOURCE_DIR;
        static bool verbose;
};

#endif