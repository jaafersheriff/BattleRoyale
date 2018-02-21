#include "ImGuiComponent.hpp"
#include "IO/Window.hpp"

void ImGuiComponent::update(float dt) {
#ifdef DEBUG_MODE
    if (Window::isImGuiEnabled()) {
        ImGui::Begin(name.c_str());
        panes();
        ImGui::End();
    }
#endif
}