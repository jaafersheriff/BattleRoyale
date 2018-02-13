#include "Window.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"

#include <iostream> /* cout, cerr */

int Window::width = DEFAULT_WIDTH;
int Window::height = DEFAULT_HEIGHT;
bool Window::imGuiEnabled = false;
float Window::imGuiTimer = 1.0;

void Window::errorCallback(int error, const char *desc) {
    std::cerr << "Error " << error << ": " << desc << std::endl;
}

void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    /* ImGui callback if it is active */
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mode);
    }
    else {
        Keyboard::setKeyStatus(key, action);
    }
}

void Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    /* ImGui callback if it is active */
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    }
    else {
        Mouse::setButtonStatus(button, action);
    }
}

void Window::characterCallback(GLFWwindow *window, unsigned int c) {
#ifdef DEBUG
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_CharCallback(window, c);
    }
#endif
}

int Window::init(std::string name) {
    /* Set error callback */
    glfwSetErrorCallback(errorCallback);

    /* Init GLFW */
    if(!glfwInit()) {
        std::cerr << "Error initializing GLFW" << std::endl;
        return 1;
    }

    /* Request version 3.3 of OpenGL */
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    /* Create GLFW window */
    window = glfwCreateWindow(this->width, this->height, name.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

#ifdef DEBUG
    /* Init ImGui */
    ImGui_ImplGlfwGL3_Init(this->window, false);
#endif

    /* Set callbacks */
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCharCallback(window, characterCallback);

    /* Init GLEW */
    glewExperimental = GL_FALSE;
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << std::endl;
        return 1;
    }
    error = glewInit();
    if (error != GLEW_OK) {
        std::cerr << "Failed to init GLEW" << std::endl;
        return 1;
    }
    glGetError();

    /* Vsync */
    glfwSwapInterval(1);

    return 0;
}

void Window::setTitle(const char *name) {
    glfwSetWindowTitle(window, name);
}

void Window::update(float dt) { 
    /* Set viewport to window size */
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    /* Don't update display if window is minimized */
    if (!width && !height) {
        return;
    }

    /* Update mouse */
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    Mouse::update(x, y);

    /* Update ImGui */
#ifdef DEBUG
    imGuiTimer += dt;
    if (Keyboard::isKeyPressed(GLFW_KEY_GRAVE_ACCENT) && 
       (Keyboard::isKeyPressed(GLFW_KEY_LEFT_SHIFT) || Keyboard::isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) &&
        imGuiTimer >= 0.5) {
        toggleImGui();
        imGuiTimer = 0.0;
    }
    ImGui_ImplGlfwGL3_NewFrame(isImGuiEnabled());
#endif
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int Window::shouldClose() { 
    return glfwWindowShouldClose(window);
}

double Window::getTime() {
    return glfwGetTime();
}

void Window::shutDown() {
    /* Clean up GLFW */
    glfwDestroyWindow(window);
    glfwTerminate();
}
