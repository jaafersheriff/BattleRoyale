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
    if (windowHandler.init()) {
        return 1;
    }

    loader.init(verbose, RESOURCE_DIR);

    lastFrameTime = runTime = (float)windowHandler.getTime();
    // TODO : set library/loader dir
    // TODO : init scene

    return 0;
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
        // TODO : update collision system
        // TODO : update game logic system
        // TODO : render 
    }
}

void EngineApp::terminate() {
    windowHandler.shutDown();
}

int EngineApp::processArgs(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h")) {
            printUsage();
            return 1;
        }
        /* Verbose */
        if (!strcmp(argv[i], "-v")) {
            verbose = true;
        }
        /* Set resource dir */
        if (!strcmp(argv[i], "-r")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            RESOURCE_DIR = argv[i + 1];
        }
        /* Set application name */
        if (!strcmp(argv[i], "-n")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            APP_NAME = argv[i + 1];
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

    std::cout << "\t-n <application_name>" << std::endl;
    std::cout << "\t\tSet the application name" << std::endl;
}