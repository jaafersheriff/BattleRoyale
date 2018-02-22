#pragma once
#ifndef _ENGINE_APP_HPP_
#define _ENGINE_APP_HPP_

#include "IO/Window.hpp"
#include "Loader/Loader.hpp"
#include "Scene/Scene.hpp"

#include <string>

// static class
class EngineApp {

    public:

        static int init();
        static void run();
        static void terminate();

        static String RESOURCE_DIR;   /* Static resource directory */
        static String APP_NAME;       /* Name of application */
        static int fps;                    /* Frames per second */
        static double timeStep;            /* Delta time */
        static bool verbose;               /* Log things or not */

    private:

        static double lastFpsTime;         /* Time at which last FPS was calculated */
        static int nFrames;                /* Number of frames in current second */
        static double lastFrameTime;       /* Time at which last frame was rendered */
        static double runTime;             /* Global timer */

};

#endif