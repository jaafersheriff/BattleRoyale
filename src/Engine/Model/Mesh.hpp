/* Mesh class
 * Contains geometry for a singular mesh */
#pragma once
#ifndef _MESH_HPP_
#define _MESH_HPP_

#include <vector>   /* vector */

class Mesh {
    public:
        struct MeshBuffers {
            std::vector<float> vertBuf;
            std::vector<float> norBuf;
            std::vector<float> texBuf;
            std::vector<unsigned int> eleBuf;
        };
        /* Constructor */
        Mesh() :
            vaoId(0),
            vertBufId(0),
            norBufId(0),
            texBufId(0),
            eleBufId(0)
        {}
        /* VAO ID */
        unsigned int vaoId;

        /* VBO IDs */
        unsigned int vertBufId;
        unsigned int norBufId;
        unsigned int texBufId;
        unsigned int eleBufId;

        /* VBO Info */
        MeshBuffers buffers;
        int vertBufSize;
        int norBufSize;
        int texBufSize;
        int eleBufSize;


};

#endif
