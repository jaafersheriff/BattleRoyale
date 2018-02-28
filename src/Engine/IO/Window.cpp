#include "Window.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw_gl3.h"

#include <iostream>

#include "Scene/Scene.hpp"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

GLFWwindow * Window::s_window = nullptr;
glm::ivec2 Window::s_frameSize;
glm::ivec2 Window::s_windowedSize = glm::ivec2(DEFAULT_WIDTH, DEFAULT_HEIGHT);
glm::ivec2 Window::s_fullscreenSize;
bool Window::s_fullscreen = false;
bool Window::s_vSyncEnabled = true;
bool Window::s_cursorEnabled = true;
bool Window::s_imGuiEnabled = false;

void Window::errorCallback(int error, const char *desc) {
    std::cerr << "Error " << error << ": " << desc << std::endl;
}

void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    static glm::ivec2 s_windowedPos;

    // exit app
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    // toggle fullscreen (f11 or alt+enter)
    if ((key == GLFW_KEY_F11 || key == GLFW_KEY_ENTER && mods & GLFW_MOD_ALT) && action == GLFW_PRESS) {
        // currently fullscreen
        if (glfwGetWindowMonitor(window)) {
            s_fullscreen = false;
            glfwSetWindowMonitor(window, nullptr, s_windowedPos.x, s_windowedPos.y, s_windowedSize.x, s_windowedSize.y, GLFW_DONT_CARE);
            Mouse::reset();
        }
        // currently windowed
        else {
            glfwGetWindowPos(window, &s_windowedPos.x, &s_windowedPos.y);
            s_fullscreen = true;
            GLFWmonitor * monitor(glfwGetPrimaryMonitor());
            const GLFWvidmode * video(glfwGetVideoMode(monitor));
            glfwSetWindowMonitor(window, monitor, 0, 0, video->width, video->height, video->refreshRate);
            Mouse::reset();
        }
        
        return;
    }

#ifdef DEBUG_MODE
    if (key == GLFW_KEY_GRAVE_ACCENT && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS) {
        toggleImGui();
    }
    else if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS && isImGuiEnabled()) {
        toggleCursorEnabled();
    }
    else if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    }
    else
#endif
    {
        if (!s_cursorEnabled) {
            Keyboard::setKeyStatus(key, action);
            Scene::sendMessage<KeyMessage>(nullptr, key, action, mods);
        }
    }
}

void Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
#ifdef DEBUG_MODE
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    }
    else 
#endif
    {
        if (!s_cursorEnabled) {
            Mouse::setButtonStatus(button, action);
            Scene::sendMessage<MouseMessage>(nullptr, button, action, mods);
        }
    }
}

void Window::scrollCallback(GLFWwindow * window, double dx, double dy) {
#ifdef DEBUG_MODE
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_ScrollCallback(window, dx, dy);
    }
    else
#endif
    {
        Mouse::scrollDX += dx;
        Mouse::scrollDY += dy;
        Scene::sendMessage<ScrollMessage>(nullptr, float(dx), float(dy));
    }
}

void Window::characterCallback(GLFWwindow *window, unsigned int c) {
#ifdef DEBUG_MODE
    if (isImGuiEnabled() && (ImGui::IsWindowFocused() || ImGui::IsMouseHoveringAnyWindow())) {
        ImGui_ImplGlfwGL3_CharCallback(window, c);
    }
#endif
}

void Window::windowSizeCallback(GLFWwindow * window, int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }

    if (s_fullscreen) {
        s_fullscreenSize.x = width;
        s_fullscreenSize.y = height;
    }
    else {
        s_windowedSize.x = width;
        s_windowedSize.y = height;
    }
}

void Window::framebufferSizeCallback(GLFWwindow * window, int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }

    s_frameSize.x = width; s_frameSize.y = height;
    /* Set viewport to window size */
    glViewport(0, 0, width, height);
    Scene::sendMessage<WindowFrameSizeMessage>(nullptr, s_frameSize);
}

void Window::cursorEnterCallback(GLFWwindow * window, int entered) {
    Mouse::reset();
}

int Window::init(const String & name) {
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
    glfwWindowHint(GLFW_AUTO_ICONIFY, false);

    /* Create GLFW window */
    s_window = glfwCreateWindow(s_windowedSize.x, s_windowedSize.y, name.c_str(), NULL, NULL);
    if (!s_window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(s_window);

    /* Init ImGui */
    ImGui_ImplGlfwGL3_Init(s_window, false);

    /* Set callbacks */
    glfwSetKeyCallback(s_window, keyCallback);
    glfwSetMouseButtonCallback(s_window, mouseButtonCallback);
    glfwSetScrollCallback(s_window, scrollCallback);
    glfwSetCharCallback(s_window, characterCallback);
    glfwSetWindowSizeCallback(s_window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(s_window, framebufferSizeCallback);
    glfwSetCursorEnterCallback(s_window, cursorEnterCallback);

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
    glfwSwapInterval(s_vSyncEnabled);

    glfwGetFramebufferSize(s_window, &s_frameSize.x, &s_frameSize.y);

    return 0;
}

void Window::setTitle(const char *name) {
    glfwSetWindowTitle(s_window, name);
}

void Window::setSize(const glm::ivec2 & size) {
    if (!s_fullscreen) {
        glfwSetWindowSize(s_window, s_windowedSize.x, s_windowedSize.y);
    }
}

void Window::toggleVSync() {
    s_vSyncEnabled = !s_vSyncEnabled;
    glfwSwapInterval(s_vSyncEnabled);
}

void Window::update(float dt) {
    /* Don't update display if window is minimized */
    if (glfwGetWindowAttrib(s_window, GLFW_ICONIFIED)) {
        return;
    }

    glfwPollEvents();

    /* Update mouse */
    if (!s_cursorEnabled) {
        double x, y;
        glfwGetCursorPos(s_window, &x, &y);
        Mouse::update(x, y);
    }

#ifdef DEBUG_MODE
    /* Update ImGui */
    if (isImGuiEnabled()) {
        ImGui_ImplGlfwGL3_NewFrame(s_cursorEnabled);
    }
#endif
    
    glfwSwapBuffers(s_window);
}

int Window::shouldClose() { 
    return glfwWindowShouldClose(s_window);
}

double Window::getTime() {
    return glfwGetTime();
}

void Window::shutDown() {
    /* Clean up GLFW */
    glfwDestroyWindow(s_window);
    glfwTerminate();
}

void Window::toggleImGui() {
    s_imGuiEnabled = !s_imGuiEnabled;
    if (!s_imGuiEnabled) {
        setCursorEnabled(false);
    }
}

void Window::setCursorEnabled(bool enabled) {
    s_cursorEnabled = enabled;
    glfwSetInputMode(s_window, GLFW_CURSOR, s_cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    if (enabled) {
        Mouse::reset();
        Keyboard::reset();
    }
}

void Window::toggleCursorEnabled() {
    setCursorEnabled(!s_cursorEnabled);
}
