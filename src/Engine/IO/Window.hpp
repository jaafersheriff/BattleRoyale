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

// static class
class Window {
    
    public:

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

        static void setSize(const glm::ivec2 & size);
        static const glm::ivec2 & getSize() { return s_fullscreen ? s_fullscreenSize : s_windowedSize; }
        static const glm::ivec2 & getFrameSize() { return s_frameSize; }
        static float getAspectRatio() { return float(s_frameSize.x) / float(s_frameSize.y); }

        static bool isFullscreen() { return s_fullscreen; }

        static void toggleVSync();
        static bool isVSyncEnabled() { return s_vSyncEnabled; }

        /* ImGui */
        static void toggleImGui();
        static bool isImGuiEnabled() { return s_imGuiEnabled; }

        static void setCursorEnabled(bool enabled);
        static void toggleCursorEnabled();

    private:
    
        /* Reference to GLFW window */
        static GLFWwindow * s_window;
        static glm::ivec2 s_frameSize;
        static glm::ivec2 s_windowedSize;
        static glm::ivec2 s_fullscreenSize;
        static bool s_fullscreen;
        static bool s_vSyncEnabled;
        static bool s_cursorEnabled;
        static bool s_imGuiEnabled;

        /* Callback functions */
        static void errorCallback(int, const char *);
        static void keyCallback(GLFWwindow *, int, int, int, int);
        static void mouseButtonCallback(GLFWwindow *, int, int, int);
        static void characterCallback(GLFWwindow *, unsigned int);
        static void windowSizeCallback(GLFWwindow *, int width, int height);
        static void framebufferSizeCallback(GLFWwindow *, int width, int height);
        static void cursorEnterCallback(GLFWwindow * window, int entered);

};

#endif
