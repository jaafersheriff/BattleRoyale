#pragma once
#ifndef _IMGUI_COMPONENT
#define _IMGUI_COMPONENT

#include "Component/Component.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"

#include <functional>

class RenderSystem;

class ImGuiComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

        ImGuiComponent(GameObject & gameObject, String name, std::function<void()> fun);

    public:

        ImGuiComponent(ImGuiComponent && other) = default;

        void update(float);

    private:

        String name;
        std::function<void()> panes;
};

#endif