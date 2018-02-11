#include "EngineApp.hpp"

#include <time.h>
#include <iostream>

EngineApp::EngineApp() {
    srand((unsigned int)(time(0)));
    RESOURCE_DIR = "../resources/";
    APP_NAME = "";
    verbose = false;
    nFrames = 0;
    timeStep = lastFrameTime = runTime = 0.0;
}

int EngineApp::init() {
    if (windowHandler.init(APP_NAME)) {
        return 1;
    }

    m_scene.reset(new Scene());
    loader.init(verbose, RESOURCE_DIR);

    lastFrameTime = runTime = windowHandler.getTime();
   
    return 0;
}

void EngineApp::run() {

    // TODO : call all active component init functions

    while (!windowHandler.shouldClose()) {
        /* Update time and FPS */
        runTime = windowHandler.getTime();
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
        m_scene->update(float(timeStep));
    }
}

void EngineApp::terminate() {
    windowHandler.shutDown();
    m_scene->shutDown();
}
