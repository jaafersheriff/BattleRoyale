#include "ImGuiComponent.hpp"

void ImGuiComponent::update(float dt) {
    ImGui::Begin(name.c_str());
    panes();
    ImGui::End();
}