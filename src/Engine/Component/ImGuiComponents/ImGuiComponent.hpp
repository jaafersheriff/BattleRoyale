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
        ImGuiComponent()
        {}

        void addPane(std::string, std::function<void(float)>);

        void update(float);

    private:
        std::map<std::string, std::vector<std::function<void(float)>>> panes;
};

#endif