#pragma once



#include "glm/glm.hpp"

#include "Memory.hpp"



class Octree {

    struct Node {

        glm::vec3 center;
        Node * nodes;
        unsigned char active;

    };

    private:

    Node * m_root;

};