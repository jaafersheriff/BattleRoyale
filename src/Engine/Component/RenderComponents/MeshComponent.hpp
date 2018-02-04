#pragma once
#ifndef _MESH_COMPONENT_HPP_
#define _MESH_COMPONENT_HPP_

#include "Component/Component.hpp"

#include "Model/Mesh.hpp"

class MeshComponent : public Component {
    public:
        MeshComponent(Mesh *m) :
            mesh(m)
        {}

        void init() {}
        void update(float) {}

        Mesh* mesh;
};

#endif