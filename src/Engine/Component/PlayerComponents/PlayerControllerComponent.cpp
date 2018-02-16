#include "PlayerControllerComponent.hpp"

#include "IO/Mouse.hpp"
#include "IO/Keyboard.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



void PlayerControllerComponent::update(float dt) {    
    if (Mouse::isDown(GLFW_MOUSE_BUTTON_1)) {
        //lookAround(dt);
    }

    int forward(Keyboard::isKeyPressed(GLFW_KEY_W));
    int backward(Keyboard::isKeyPressed(GLFW_KEY_S));
    int left(Keyboard::isKeyPressed(GLFW_KEY_A));
    int right(Keyboard::isKeyPressed(GLFW_KEY_D));
    int up(Keyboard::isKeyPressed(GLFW_KEY_SPACE));
    int down(Keyboard::isKeyPressed(GLFW_KEY_LEFT_SHIFT));
    
    glm::vec3 dir(
        float(right - left),
        float(up - down),
        float(backward - forward)
    );
}