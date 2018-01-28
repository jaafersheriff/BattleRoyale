// Enemy class to work on pathfinding


#pragma once

#include <iostream>
#include "Entity.hpp"
#include <GLFW/glfw3.h>


class Enemy : public Entity {

public:
    Enemy(Mesh *, ModelTexture, const glm::vec3, const glm::vec3, const glm::vec3);

    void update(std::vector<Entity *> entities);

    glm::vec3 playerPos;
    bool hit;
};