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

    friend Scene;

    protected: // only scene can create component

        ImGuiComponent(std::string name, std::function<void()> fun) :
            name(name),
            panes(fun)
        {}

    public:

        virtual SystemID systemID() const override { return SystemID::gameLogic; };

        void update(float);

    private:
        std::string name;
        std::function<void()> panes;
};

#endif