#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "Component/RenderComponents/DiffuseRenderComponent.hpp"



void RenderSystem::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderSystem::update(float dt) {\
    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    /* Loop through active shaders */
    for (auto &shader : m_shaders) {
        if (!shader.second->isEnabled()) {
            continue;
        }

        shader.second->bind();
        ///////////////////////////  TODO  ///////////////////////////
        // pass a list of render components that are specific       //
        // to this shader -- right now we are passing the entire    //
        // list and expecting each shader to filter through         //
        //////////////////////////////////////////////////////////////

        shader.second->render(m_componentRefs);
        shader.second->unbind();
    }

    /* ImGui */
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
}

void RenderSystem::add(std::unique_ptr<Component> component) {
    m_componentRefs.push_back(component.get());
    if (dynamic_cast<DiffuseRenderComponent *>(component.get()))
        m_diffuseComponents.emplace_back(static_cast<DiffuseRenderComponent *>(component.release()));
}

void RenderSystem::remove(Component * component) {
    if (dynamic_cast<DiffuseRenderComponent *>(component)) {
        for (auto it(m_diffuseComponents.begin()); it != m_diffuseComponents.end(); ++it) {
            if (it->get() == component) {
                m_diffuseComponents.erase(it);
                break;
            }
        }
    }
    // remove from refs
    for (auto it(m_componentRefs.begin()); it != m_componentRefs.end(); ++it) {
        if (*it == component) {
            m_componentRefs.erase(it);
            break;
        }
    }
}