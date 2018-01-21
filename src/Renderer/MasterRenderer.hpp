/* Master Renderer class
 * World class activates needed subrenderers which are organized here
 * Master Renderer only renders active subrenderers  */
#pragma once
#ifndef _MASTER_RENDERER_HPP_
#define _MASTER_RENDERER_HPP_

#include "Context/Context.hpp"
#include "Context/Display.hpp"

#include "glm/glm.hpp"

#include <vector>

class World;
class Shader;
class Entity;

class MasterRenderer {
    public:
        /* Enum for shader types*/
        enum ShaderTypes {
            ERROR,
            ENTITY_SHADER,
        };
        /* List of active renderers */
        std::vector<Shader *> shaders;

        /* Activate subrenderers */
        void activateEntityShader(std::vector<Entity *> *);

        /* Init */
        void init(const Context &);

        /* Render function */
        void render(const World *);

        /* Wrap up */
        void cleanUp();

        /* Utility functions */
        void toggleWireFrameMode();

    private:
        /* Utility functions */
        void bindWorldUniforms(const World *, Shader *);

        /* Utility members */
        bool wireFrame = false;
        bool verbose = false;
};

#endif
