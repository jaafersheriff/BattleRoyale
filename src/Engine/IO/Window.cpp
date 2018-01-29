#include "Window.hpp"

#include <iostream> /* cout, cerr */

void Window::errorCallback(int error, const char *desc) {
    std::cerr << "Error " << error << ": " << desc << std::endl;
}

int Window::init() {
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
    window = glfwCreateWindow(this->width, this->height, "Neo", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    mouse.window = keyboard.window = window;

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

void Window::update() { 
    /* Set viewport to window size */
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    /* Don't update display if window is minimized */
    if (!width && !height) {
        return;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int Window::shouldClose() { 
    return glfwWindowShouldClose(window) || 
           glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

double Window::getTime() {
    return glfwGetTime();
}

void Window::shutDown() {
    /* Clean up GLFW */
    glfwDestroyWindow(window);
    glfwTerminate();
}
