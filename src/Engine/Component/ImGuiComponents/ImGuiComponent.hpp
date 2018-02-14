#pragma once
#ifndef _IMGUI_COMPONENT
#define _IMGUI_COMPONENT

#include "Component/Component.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"

#include <string>
#include <map>
#include <functional>

class GameLogicSystem;

class ImGuiComponent : public Component {

    public:

    using SystemClass = GameLogicSystem;

    public:
        ImGuiComponent(std::string name, std::function<void()> fun) :
            name(name),
            panes(fun)
        {}

        void update(float);

    private:
        std::string name;
        std::function<void()> panes;
};

#endif