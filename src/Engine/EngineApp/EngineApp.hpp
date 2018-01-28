#pragma once
#ifndef _ENGINE_APP_HPP_
#define _ENGINE_APP_HPP_

#include <string>

class EngineApp {
    public:
        EngineApp();

        void processArgs(int, char**);
        void init();
        void run();

        std::string RESOURCE_DIR;   /* Static resource directory */
        double timeStep;            /* Delta time */
        bool verbose;               /* Log things or not */

    private:
        double fps;                 /* Frames per second */
        double lastFpsTime;         /* Time at which last FPS was calculated */
        int nFrames;                /* Number of frames in current second */
        double lastFrameTime;       /* Time at which last frame was rendered */
        double runTime;             /* Global timer */

        GLFWHandler windowHandler;
        void printUsage();
};

#endif