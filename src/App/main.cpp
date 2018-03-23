// allows program to be run on dedicated graphics processor for laptops with
// both integrated and dedicated graphics using Nvidia Optimus
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif


#include <cstring>
#include <iostream>

#include "glm/gtx/transform.hpp"

#include "EngineApp/EngineApp.hpp"
#include "Scene/Scene.hpp"

void printUsage() {
    std::cout << "Valid arguments: " << std::endl;

    std::cout << "\t-h\n\t\tPrint help" << std::endl;
    
    std::cout << "\t-v\n\t\tSet verbose nature logging" << std::endl;

    std::cout << "\t-r <resource_dir>" << std::endl;
    std::cout << "\t\tSet the resource directory" << std::endl;

    std::cout << "\t-n <application_name>" << std::endl;
    std::cout << "\t\tSet the application name" << std::endl;

    std::cout << "\t-m <file_name>" << std::endl;
    std::cout << "\t\tCreate graph representation of the map, filename of said graph" << std::endl;
}

int parseArgs(int argc, char **argv) {
    /* Process cmd line args */
    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h")) {
            printUsage();
            return 1;
        }
        /* Verbose */
        if (!strcmp(argv[i], "-v")) {
            EngineApp::verbose = true;
        }
        /* Set resource dir */
        if (!strcmp(argv[i], "-r")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::RESOURCE_DIR = argv[i + 1];
        }
        /* Set application name */
        if (!strcmp(argv[i], "-n")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::APP_NAME = argv[i + 1];
        }

        /* Create graph of map for pathfinding */
        if (!strcmp(argv[i], "-m")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            Scene::mapFilename = argv[i + 1];
            Scene::mapping = true;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (parseArgs(argc, argv) || EngineApp::init()) {
        std::cin.get(); // don't immediately close the console

        return EXIT_FAILURE; 
    }

    EngineApp::run();

    return EXIT_SUCCESS;
}
