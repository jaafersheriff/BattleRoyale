/* GLFW Handler
 * Maintains GLFW window, mouse, and keyboard*/
#pragma once
#ifndef _GLFW_HANDLER_HPP_
#define _GLFW_HANDLER_HPP_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include "Util/Memory.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

class Window {
    public:
       /* Reference to GLFW window, mouse, keyboard */
        static GLFWwindow * window;

        /* Init */
        static int init(const String &);

        /* Set window title */
        static void setTitle(const char *);

        /* Update */
        static void update(float);

        /* Return if window should close */
        static int shouldClose();

        /* Return running time */
        static double getTime();

        /* Shut down */
        static void shutDown();

        /* Set window size */
        static void setSize(int w, int h);
        static glm::ivec2 getSize();
        static float getAspectRatio();

        static void toggleVSync();
        static bool isVSyncEnabled() { return vSyncEnabled; }

        /* ImGui */
        static void toggleImGui();
        static bool isImGuiEnabled() { return imGuiEnabled; }

        static void setCursorEnabled(bool enabled);
        static void toggleCursorEnabled();

    private:
    
        static bool vSyncEnabled;
        static bool cursorEnabled;

        /* ImGui */
        static bool imGuiEnabled;

        /* Callback functions */
        static void errorCallback(int, const char *);
        static void keyCallback(GLFWwindow *, int, int, int, int);
        static void mouseButtonCallback(GLFWwindow *, int, int, int);
        static void characterCallback(GLFWwindow *, unsigned int);
        static void framebufferSizeCallback(GLFWwindow *, int width, int height);
        static void cursorEnterCallback(GLFWwindow * window, int entered);
};

#endif
