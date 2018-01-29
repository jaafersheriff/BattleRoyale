#include "Mouse.hpp"

void Mouse::update() {
    /* Get new x-y positions on screen */
    double newX, newY;
    glfwGetCursorPos(window, &newX, &newY);

    /* Calculate x-y speed */
    dx = newX - this->x;
    dy = newY - this->y;

    /* Set new positions */
    // TODO: if newX > 0 and newY > 0
    this->x= newX;
    this->y= newY;

    // TODO : dw = scroll whell
}

bool Mouse::isLeftPressed() {
    return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
}

bool Mouse::isRightPressed() {
    return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
}
