// allows program to be run on dedicated graphics processor for laptops with
// both integrated and dedicated graphics using Nvidia Optimus
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif

#include "EngineApp/EngineApp.hpp"
#include "Shaders/BounderShader.hpp"

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

    if (parseArgs(&engine, argc, argv) || engine.init()) {
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    /* Scene reference for QOL */
    Scene & scene(*engine.scene());

    /* Create camera and camera controller components */
    GameObject & camera(scene.createGameObject());
    CameraComponent & cc(scene.createComponent<CameraComponent>(45.f, 0.01f, 250.f));
    camera.addComponent(cc);
    camera.addComponent(scene.createComponent<CameraController>(cc, 0.2f, 15.f, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT));
    camera.addComponent(scene.createComponent<SpatialComponent>());
    camera.getSpatial()->setPosition(glm::vec3(-4.0f, 0.0f, 0.0f));

    /* Create diffuse shader */
    glm::vec3 lightPos(100.f, 100.f, 100.f);
    // TODO : user shouldn't need to specify resource dir here
    if (!scene.renderSystem().addShader<DiffuseShader>(
            "diffuse",                                    /* Shader name              */
            engine.RESOURCE_DIR + "diffuse_vert.glsl",    /* Vertex shader file       */
            engine.RESOURCE_DIR + "diffuse_frag.glsl",    /* Fragment shader file     */
            cc,                                           /* Shader-specific uniforms */
            lightPos                                      /*                          */
        )) {
        std::cerr << "Failed to add diffuse shader" << std::endl;
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    // Create collider shader
    // alternate method using unique_ptr and new
    if (!scene.renderSystem().addShader("bounder", std::unique_ptr<BounderShader>(new BounderShader(
            engine.RESOURCE_DIR + "bounder_vert.glsl",
            engine.RESOURCE_DIR + "bounder_frag.glsl",
            scene.collisionSystem(),
            cc
        )))) {
        std::cerr << "Failed to add collider shader" << std::endl;
        std::cin.get(); //don't immediately close the console
        return EXIT_FAILURE;
    }
    static_cast<BounderShader *>(scene.renderSystem().getShader("bounder"))->enable();

    /* Create bunny */
    Mesh * bunnyMesh(Loader::getMesh("bunny.obj"));
    GameObject & bunny(scene.createGameObject());
    bunny.addComponent(scene.createComponent<SpatialComponent>(
        glm::vec3(0.0f, 0.0f, 0.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f), // scale
        glm::mat3() // rotation
    ));
    bunny.addComponent(scene.addComponent<BounderComponent>(createBounderFromMesh(0, *bunnyMesh, true, true, true)));
    bunny.addComponent(scene.createComponent<DiffuseRenderComponent>(
        scene.renderSystem().m_shaders.find("diffuse")->second->pid,
        *Loader::getMesh("bunny.obj"),
        ModelTexture(0.3f, glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f))));
                            

    /* Main loop */
    engine.run();

    scene.shutDown();

    return EXIT_SUCCESS;
}


