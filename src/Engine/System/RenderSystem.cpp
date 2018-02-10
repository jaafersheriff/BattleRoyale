#include "RenderSystem.hpp"

RenderSystem::RenderSystem(std::vector<Component *> & components) :
    System(components)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderSystem::update(float dt) {
    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    /* Loop through active shaders */
    for (auto &shader : shaders) {
        shader.second->bind();
        ///////////////////////////  TODO  ///////////////////////////
        // pass a list of render components that are specific       //
        // to this shader -- right now we are passing the entire    //
        // list and expecting each shader to filter through         //
        //////////////////////////////////////////////////////////////
        shader.second->render(shader.first, components);
        shader.second->unbind();
    }
}