#include "Window.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"

#include <iostream> /* cout, cerr */

#include "Scene/Scene.hpp"

GLFWwindow * Window::window = nullptr;
bool Window::vSyncEnabled = true;
bool Window::cursorEnabled = true;
bool Window::imGuiEnabled = false;
float Window::imGuiTimer = 1.0;

void Window::errorCallback(int error, const char *desc) {
    std::cerr << "Error " << error << ": " << desc << std::endl;
}

void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    /* ImGui callback if it is active */
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    }
    else {
        Keyboard::setKeyStatus(key, action);
        Scene::get().sendMessage<KeyMessage>(nullptr, key, action, mods);
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
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_CharCallback(window, c);
    }
}

void Window::framebufferSizeCallback(GLFWwindow * window, int width, int height) {
    /* Set viewport to window size */
    glViewport(0, 0, width, height);
    Scene::get().sendMessage<WindowSizeMessage>(nullptr, width, height);
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
    window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, name.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    /* Init ImGui */
    ImGui_ImplGlfwGL3_Init(window, false);

    /* Set callbacks */
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCharCallback(window, characterCallback);
    glfwSetWindowSizeCallback(window, framebufferSizeCallback);

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
    glfwSwapInterval(vSyncEnabled);

    return 0;
}

void Window::setTitle(const char *name) {
    glfwSetWindowTitle(window, name);
}

void Window::setSize(int w, int h) {
    glfwSetWindowSize(window, w, h);
}

glm::ivec2 Window::getSize() {
    glm::ivec2 size;
    glfwGetFramebufferSize(window, &size.x, &size.y);
    return size;
}

float Window::getAspectRatio() {
    glm::ivec2 size(getSize());
    return float(size.x) / float(size.y);
}

void Window::toggleVSync() {
    vSyncEnabled = !vSyncEnabled;
    glfwSwapInterval(vSyncEnabled);
}

void Window::update(float dt) {
    /* Don't update display if window is minimized */
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
        return;
    }

    /* Update mouse */
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    Mouse::update(x, y);

    /* Update ImGui */
    // TODO: clean up this code
    static bool imGuiActive = false;
    static bool priorCursorState;
    imGuiTimer += dt;
    if (Keyboard::isKeyPressed(GLFW_KEY_GRAVE_ACCENT) && imGuiTimer >= 0.5) {
        if (!imGuiActive) {
            priorCursorState = cursorEnabled;
            setCursorEnabled(true);
        }

        toggleImGui();
        imGuiTimer = 0.0;
        imGuiActive = !imGuiActive;
        ImGui_ImplGlfwGL3_NewFrame(isImGuiEnabled());

        if (!imGuiActive) {
            setCursorEnabled(priorCursorState);
        }
    }
    else {
        ImGui_ImplGlfwGL3_NewFrame(isImGuiEnabled());
    }
    
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

void Window::setCursorEnabled(bool enabled) {
    cursorEnabled = enabled;
    glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}
