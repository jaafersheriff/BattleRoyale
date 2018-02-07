#include "EngineApp/EngineApp.hpp"

#include <string>
#include <iostream>

void printUsage() {
    std::cout << "Valid arguments: " << std::endl;

    std::cout << "\t-h\n\t\tPrint help" << std::endl;
    
    std::cout << "\t-v\n\t\tSet verbose nature logging" << std::endl;

    std::cout << "\t-r <resource_dir>" << std::endl;
    std::cout << "\t\tSet the resource directory" << std::endl;

    std::cout << "\t-n <application_name>" << std::endl;
    std::cout << "\t\tSet the application name" << std::endl;
}

int parseArgs(EngineApp *engine, int argc, char **argv) {
    /* Process cmd line args */
    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h")) {
            printUsage();
            return 1;
        }
        /* Verbose */
        if (!strcmp(argv[i], "-v")) {
            engine->verbose = true;
        }
        /* Set resource dir */
        if (!strcmp(argv[i], "-r")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            engine->RESOURCE_DIR = argv[i + 1];
        }
        /* Set application name */
        if (!strcmp(argv[i], "-n")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            engine->APP_NAME = argv[i + 1];
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    /* Singular engine */
    EngineApp engine;
    Scene *scene = &engine.scene;

    if (parseArgs(&engine, argc, argv)) {
        return 1;
    }

    /* Initialize engine */
    if (engine.init()) {
        return 1;
    }

    /* Create camera */
    GameObject *camera = scene->createGameObject();
    CameraComponent *cc = scene->createComponent<Scene::GAMELOGIC, CameraComponent>(45.f, 1280.f / 960.f, 0.01f, 250.f);
    camera->addComponent(cc);
    camera->addComponent(scene->createComponent<Scene::GAMELOGIC, CameraController>(cc, 20.f, 30.f, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_E, GLFW_KEY_R));

    /* Create diffuse shader */
    scene->renderer->addShader<DiffuseShader>("diffuse", engine.RESOURCE_DIR + "diffuse_vert.glsl", engine.RESOURCE_DIR + "diffuse_frag.glsl");

    /* Main loop */
    engine.run();

    return 0;
}


