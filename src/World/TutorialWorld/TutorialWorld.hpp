/* Basic world for general development
 * Derives World interface */
#pragma once
#ifndef _TUTORIAL_WORLD_HPP_
#define _TUTORIAL_WORLD_HPP_

#include <iostream>

#include "World/World.hpp"
#include "Toolbox/Toolbox.hpp"

#include "Entity/Entity.hpp"
#include "Entity/Enemy.hpp"

class TutorialWorld : public World {
    public:
        /* World-specific render targets */
        std::vector<Entity *> entities;

        /* World-specific members */
        Light *light;
        Camera *camera;
        glm::mat4 P;
        glm::mat4 V;
        bool isPaused = false;

        /* Constructor */
        TutorialWorld() : World("Tutorial World") { }

        /* Derived functions */
        void init(Context &, Loader &);
        void prepareRenderer(MasterRenderer *);
        void prepareUniforms();
        void update(Context &);
        void takeInput(Mouse &, Keyboard &);
        void cleanUp();
};

#endif
