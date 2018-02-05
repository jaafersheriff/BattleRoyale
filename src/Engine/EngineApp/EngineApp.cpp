#include "EngineApp.hpp"

#include <time.h>
#include <iostream>

EngineApp::EngineApp() {
    srand(time(0));
    RESOURCE_DIR = "../resources/";
    APP_NAME = "";
    verbose = false;
    nFrames = 0;
    timeStep = lastFrameTime = runTime = 0.f;
}

int EngineApp::init() {
    if (windowHandler.init(APP_NAME)) {
        return 1;
    }

    loader.init(verbose, RESOURCE_DIR);

    lastFrameTime = runTime = (float)windowHandler.getTime();

    return 0;
}

void EngineApp::run() {

    // TODO : call all active component init functions

    while (!windowHandler.shouldClose()) {
        /* Update time and FPS */
        runTime = (float)windowHandler.getTime();
        timeStep = (runTime - lastFrameTime);
        lastFrameTime = runTime;
        nFrames++;
        if (runTime - lastFpsTime >= 1.0) {
            fps = (double)nFrames;
            nFrames = 0;
            lastFpsTime = runTime;
        }

        /* Update display, mouse, and keyboard */
        windowHandler.update();

        /* Update all game objects and components */
        scene.update(timeStep);
    }
}

void EngineApp::terminate() {
    windowHandler.shutDown();
    scene.shutDown();
}
