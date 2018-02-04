/* Transformation class */
#pragma once 
#ifndef _TRANSFORM_HPP_
#define _TRANSFORM_HPP_

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// TODO : only update matrices after pos, rot, or scale has been updated
// TODO : each member var needs its own getter and setter in that case
class Transform {
    public:
        Transform() :
            position(glm::vec3(0.f)),
            rotation(glm::vec3(0.f)),
            scale(glm::vec3(0.f)),
            modelMatrix(glm::mat4(1.f))
        {}

        void update() {
            this->modelMatrix  = glm::mat4(1.f);
            this->modelMatrix *= glm::translate(glm::mat4(1.f), position);
            this->modelMatrix *= glm::rotate(glm::mat4(1.f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
            this->modelMatrix *= glm::rotate(glm::mat4(1.f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
            this->modelMatrix *= glm::rotate(glm::mat4(1.f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
            this->modelMatrix *= glm::scale(glm::mat4(1.f), scale);
        }

        glm::vec3 position, rotation, scale;
        glm::mat4 modelMatrix;
};

#endif