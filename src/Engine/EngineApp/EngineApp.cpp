#include "EngineApp.hpp"

#include <time.h>
#include <iostream>


std::string EngineApp::RESOURCE_DIR = "../resources/";
std::string EngineApp::APP_NAME = "Battle Royale with Cheese";
int EngineApp::fps = 0;
double EngineApp::timeStep = 0.0;
bool EngineApp::verbose = false;
double EngineApp::lastFpsTime = 0.0;
int EngineApp::nFrames = 0;
double EngineApp::lastFrameTime = 0.0;
double EngineApp::runTime = 0.0;

int EngineApp::init() {
    srand((unsigned int)(time(0)));    
    if (Window::init(APP_NAME)) {
        return 1;
    }

    Scene::init();
    Loader::init(verbose, RESOURCE_DIR);

    lastFrameTime = runTime = Window::getTime();
   
    return 0;
}

void EngineApp::run() {

    while (!Window::shouldClose()) {
        /* Update time and FPS */
        runTime = Window::getTime();
        timeStep = glm::min((runTime - lastFrameTime), 0.25); // right now physics/collisions break when dt is too large, so this is a workaround
        lastFrameTime = runTime;
        nFrames++;
        if (runTime - lastFpsTime >= 1.0) {
            fps = nFrames;
            nFrames = 0;
            lastFpsTime = runTime;
        }

        /* Update display, mouse, and keyboard */
        Window::update(float(timeStep));

        /* Update all game objects and components */
        Scene::update(float(timeStep));
    }
}

void EngineApp::terminate() {
    Window::shutDown();
}
