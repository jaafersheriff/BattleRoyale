#include "Loader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "ThirdParty/tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb_image.h"

#include <iostream>

#include "Util/Memory.hpp"
#include "FileReader.hpp"

bool Loader::verbose = false;
String Loader::RESOURCE_DIR = "../resources/";

void Loader::init(bool verbose, const String & res) {
    verbose = verbose;
    RESOURCE_DIR = res;
}

Mesh* Loader::getMesh(const String & name) {
    Mesh* mesh = Library::getMesh(name);
    if (mesh) {
        return mesh;
    }

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> objMaterials;
    std::string errString;
    bool rc = tinyobj::LoadObj(shapes, objMaterials, errString, (RESOURCE_DIR + name).c_str());
    if (!rc) {
        std::cerr << errString << std::endl;
        exit(1);
    }
 
    /* Create a new empty mesh */
    mesh = new Mesh;

    int vertCount = 0;
    /* For every shape in the loaded file */
    for (unsigned int i = 0; i < shapes.size(); i++) {
        /* Concatenate the shape's vertices, normals, and textures to the mesh */
        mesh->buffers.vertBuf.insert(mesh->buffers.vertBuf.end(), shapes[i].mesh.positions.begin(), shapes[i].mesh.positions.end());
        mesh->buffers.norBuf.insert(mesh->buffers.norBuf.end(), shapes[i].mesh.normals.begin(), shapes[i].mesh.normals.end());
        mesh->buffers.texBuf.insert(mesh->buffers.texBuf.end(), shapes[i].mesh.texcoords.begin(), shapes[i].mesh.texcoords.end());

        /* Concatenate the shape's indices to the new mesh
         * Indices need to be incremented as we concatenate shapes */
        for (unsigned int i : shapes[i].mesh.indices) {
            mesh->buffers.eleBuf.push_back(i + vertCount);
        }
        vertCount += int(shapes[i].mesh.positions.size()) / 3;
    }

    /* Provide VBO info */
    mesh->vertBufSize = int(mesh->buffers.vertBuf.size());
    mesh->norBufSize  = int(mesh->buffers.norBuf.size());
    mesh->texBufSize  = int(mesh->buffers.texBuf.size());
    mesh->eleBufSize  = int(mesh->buffers.eleBuf.size());

    /* Resize the mesh to be centered around origin and have vertex values [-1.0, 1.0] */
    //resize(mesh->buffers);

    /* Add new mesh to library */
    Library::addMesh(name, *mesh);
    
    /* Load mesh to GPU */
    loadMesh(*mesh);

    if (verbose) {
        std::cout << "Loaded mesh (" << vertCount << " vertices): " << name << std::endl;
    }

    return mesh;
}

uint8_t* Loader::loadTextureData(const String & fileName, const bool flip, int *width, int *height, int *comp) {
    uint8_t *data;

    stbi_set_flip_vertically_on_load(flip);
    data = stbi_load((RESOURCE_DIR + fileName).c_str(), width, height, comp, STBI_rgb_alpha);

    if (data) {
        if (verbose) {
            std::cout << "Loaded texture (" << *width << ", " << *height << "): " << fileName << std::endl;
        }
    }
    else {
        std::cerr << "Could not find texture file " << RESOURCE_DIR << fileName << std::endl;
    }
    return data;
}


Texture* Loader::getTexture(const String & name, GLenum mode, bool flip) {
    Texture *texture = Library::getTexture(name);
    if (texture) {
        return texture;
    }

    texture = new Texture;
    uint8_t *data = loadTextureData(RESOURCE_DIR + name, flip, &texture->width, &texture->height, &texture->components);
    if(data) {
        loadTexture(texture, data, mode);
        if (texture->textureId) {
            Library::addTexture(name, texture);
        }
        stbi_image_free(data);
    }
    return texture;
}

Texture* Loader::getTexture(const String & name) {
    return getTexture(name, GL_REPEAT, true);
}

int Loader::loadLevel(const String & name, float ambience) {
    return FileReader::loadLevel(*name.c_str(), ambience);
}

/* Provided function to resize a mesh so all vertex positions are [0, 1.f] */
void Loader::resize(Mesh::MeshBuffers & buffers) {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float scaleX, scaleY, scaleZ;
    float shiftX, shiftY, shiftZ;
    float epsilon = 0.001f;

    minX = minY = minZ = 1.1754E+38F;
    maxX = maxY = maxZ = -1.1754E+38F;

    //Go through all vertices to determine min and max of each dimension
    for (size_t v = 0; v < buffers.vertBuf.size() / 3; v++) {
        if(buffers.vertBuf[3*v+0] < minX) minX = buffers.vertBuf[3*v+0];
        if(buffers.vertBuf[3*v+0] > maxX) maxX = buffers.vertBuf[3*v+0];

        if(buffers.vertBuf[3*v+1] < minY) minY = buffers.vertBuf[3*v+1];
        if(buffers.vertBuf[3*v+1] > maxY) maxY = buffers.vertBuf[3*v+1];

        if(buffers.vertBuf[3*v+2] < minZ) minZ = buffers.vertBuf[3*v+2];
        if(buffers.vertBuf[3*v+2] > maxZ) maxZ = buffers.vertBuf[3*v+2];
    }

    //From min and max compute necessary scale and shift for each dimension
    float maxExtent, xExtent, yExtent, zExtent;
    xExtent = maxX-minX;
    yExtent = maxY-minY;
    zExtent = maxZ-minZ;
    if (xExtent >= yExtent && xExtent >= zExtent) {
        maxExtent = xExtent;
    }
    if (yExtent >= xExtent && yExtent >= zExtent) {
        maxExtent = yExtent;
    }
    if (zExtent >= xExtent && zExtent >= yExtent) {
        maxExtent = zExtent;
    }
    scaleX = 2.f /maxExtent;
    shiftX = minX + (xExtent/ 2.f);
    scaleY = 2.f / maxExtent;
    shiftY = minY + (yExtent / 2.f);
    scaleZ = 2.f/ maxExtent;
    shiftZ = minZ + (zExtent)/2.f;

    //Go through all verticies shift and scale them
    for (size_t v = 0; v < buffers.vertBuf.size() / 3; v++) {
        buffers.vertBuf[3*v+0] = (buffers.vertBuf[3*v+0] - shiftX) * scaleX;
        assert(buffers.vertBuf[3*v+0] >= -1.0 - epsilon);
        assert(buffers.vertBuf[3*v+0] <= 1.0 + epsilon);
        buffers.vertBuf[3*v+1] = (buffers.vertBuf[3*v+1] - shiftY) * scaleY;
        assert(buffers.vertBuf[3*v+1] >= -1.0 - epsilon);
        assert(buffers.vertBuf[3*v+1] <= 1.0 + epsilon);
        buffers.vertBuf[3*v+2] = (buffers.vertBuf[3*v+2] - shiftZ) * scaleZ;
        assert(buffers.vertBuf[3*v+2] >= -1.0 - epsilon);
        assert(buffers.vertBuf[3*v+2] <= 1.0 + epsilon);
    }
}

void Loader::loadTexture(Texture *texture, uint8_t *data, GLenum mode) {
    /* Set active texture unit 0 */
    glActiveTexture(GL_TEXTURE0);

    /* Generate texture buffer object */
    glGenTextures(1, &texture->textureId);

    /* Bind new texture buffer object to active texture */
    glBindTexture(GL_TEXTURE_2D, texture->textureId);

    /* Load texture data to GPU */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    /* Generate image pyramid */
    glGenerateMipmap(GL_TEXTURE_2D);

    /* Set filtering mode for magnification and minimification */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    /* Set wrap mode */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        
    /* LOD */
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.5f);

    /* Unbind */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Error check */
    assert(glGetError() == GL_NO_ERROR);
}

void Loader::loadMesh(Mesh & mesh) {
    /* Initialize VAO */
    glGenVertexArrays(1, &mesh.vaoId);
    glBindVertexArray(mesh.vaoId);

    /* Copy vertex array */
    glGenBuffers(1, &mesh.vertBufId);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertBufId);
    glBufferData(GL_ARRAY_BUFFER, mesh.buffers.vertBuf.size() * sizeof(float), &mesh.buffers.vertBuf[0], GL_STATIC_DRAW);

    /* Copy element array if it exists */
    if (!mesh.buffers.eleBuf.empty()) {
        glGenBuffers(1, &mesh.eleBufId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eleBufId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers.eleBuf.size() * sizeof(unsigned int), &mesh.buffers.eleBuf[0], GL_STATIC_DRAW);
    }

    /* Copy normal array if it exists */
    if (!mesh.buffers.norBuf.empty()) {
        glGenBuffers(1, &mesh.norBufId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.norBufId);
        glBufferData(GL_ARRAY_BUFFER, mesh.buffers.norBuf.size() * sizeof(float), &mesh.buffers.norBuf[0], GL_STATIC_DRAW);
    }

    /* Copy texture array if it exists */
    if (!mesh.buffers.texBuf.empty()) {
        glGenBuffers(1, &mesh.texBufId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.texBufId);
        glBufferData(GL_ARRAY_BUFFER, mesh.buffers.texBuf.size() * sizeof(float), &mesh.buffers.texBuf[0], GL_STATIC_DRAW);
    }

    /* Unbind  */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Error check */
    assert(glGetError() == GL_NO_ERROR);
}