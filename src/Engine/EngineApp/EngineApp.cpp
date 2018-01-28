#include "EngineApp.hpp"

#include <time.h>
#include <iostream>

EngineApp::EngineApp() {
    srand(time(0));
    RESOURCE_DIR = "../resources/";
    verbose = false;
    nFrames = 0;
    timeStep = lastFrameTime = runTime = 0.f;
}

void EngineApp::init() {
    windowHandler.init();
    lastFrameTime = runTime = (float)windowHandler.getTime();
    // TODO : set library/loader dir
    // TODO : init scene
}

void EngineApp::run() {
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

        /* Update display */
        windowHandler.update();
        // TODO : update IO system
        // TODO : update collision system
        // TODO : update game logic system
        // TODO : render 
    }
}

void EngineApp::processArgs(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h")) {
            printUsage();
            exit(0);
        }
        /* Verbose */
        if (!strcmp(argv[i], "-v")) {
            verbose = true;
        }
        /* Set resource dir */
        if (!strcmp(argv[i], "-r")) {
            if (i + 1 >= argc) {
                printUsage();
                exit(1);
            }
            RESOURCE_DIR = argv[i + 1];
        }
    } 
    
}

void EngineApp::printUsage() {
    std::cout << "Valid arguments: " << std::endl;
    std::cout << "\t-h\n\t\tPrint help" << std::endl;
    std::cout << "\t-v\n\t\tSet verbose nature logging" << std::endl;
    std::cout << "\t-r <resource_dir>" << std::endl;
    std::cout << "\t\tSet the resource directory" << std::endl;
    std::cout << "\t\tDefault: " << RESOURCE_DIR << std::endl;
}