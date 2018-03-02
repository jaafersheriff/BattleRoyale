#include "ImGuiComponent.hpp"
#include "IO/Window.hpp"

ImGuiComponent::ImGuiComponent(GameObject & gameObject, String name, std::function<void()> fun) :
    Component(gameObject),
    name(name),
    panes(fun)
{}

void ImGuiComponent::update(float dt) {
#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Begin(name.c_str());
        panes();
        ImGui::End();
    }
#endif
}