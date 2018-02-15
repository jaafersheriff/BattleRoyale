// allows program to be run on dedicated graphics processor for laptops with
// both integrated and dedicated graphics using Nvidia Optimus
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif

#include <string>
#include <iostream>

#include "glm/gtx/transform.hpp"

#include "EngineApp/EngineApp.hpp"
#include "LevelBuilder/FileReader.hpp"

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
    camera.getSpatial()->setPosition(glm::vec3(-4.0f, 6.0f, 0.0f));

    Mesh *cyliderMesh(Loader::getMesh("cylinder.obj"));
    camera.addComponent(scene.createComponent<SphereBounderComponent>(1, Sphere(glm::vec3(0, 0, 0), 4)));

    /* Create diffuse shader */
    glm::vec3 lightPos(100.f, 100.f, 100.f);
    // TODO : user shouldn't need to specify resource dir here
    if (!scene.renderSystem().createShader<DiffuseShader>(
            engine.RESOURCE_DIR + "diffuse_vert.glsl",    /* Vertex shader file       */
            engine.RESOURCE_DIR + "diffuse_frag.glsl",    /* Fragment shader file     */
            cc,                                           /* Shader-specific uniforms */
            lightPos                                      /*                          */
        )) {
        std::cerr << "Failed to add diffuse shader" << std::endl;
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Diffuse Shader ImGui Pane */
    scene.createComponent<ImGuiComponent>(
        "Diffuse Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                scene.renderSystem().getShader<DiffuseShader>()->toggleEnabled();
            }
            if (ImGui::Button("Wireframe")) {
                scene.renderSystem().getShader<DiffuseShader>()->toggleWireFrame();
            }
       }
    );

    // Create collider
    // alternate method using unique_ptr and new
    if (!scene.renderSystem().addShader(std::unique_ptr<BounderShader>(new BounderShader(
            engine.RESOURCE_DIR + "bounder_vert.glsl",
            engine.RESOURCE_DIR + "bounder_frag.glsl",
            scene.collisionSystem(),
            cc
        )))) {
        std::cerr << "Failed to add collider shader" << std::endl;
        std::cin.get(); //don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Collider ImGui pane */
    scene.createComponent<ImGuiComponent>(
        "Bounder Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                scene.renderSystem().getShader<BounderShader>()->toggleEnabled();
            }
        }
    );


    /*Parse and load json level*/
    FileReader fileReader;
    const char *levelPath = "../resources/GameLevel_02.json";
    fileReader.loadLevel(*levelPath, scene);

    /* Create bunny */
    Mesh * bunnyMesh(Loader::getMesh("bunny.obj"));
    GameObject & bunny(scene.createGameObject());
    bunny.addComponent(scene.createComponent<SpatialComponent>(
        glm::vec3(0.0f, 0.0f, 0.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f), // scale
        glm::mat3() // rotation
    ));
    bunny.addComponent(scene.addComponent<BounderComponent>(createBounderFromMesh(1, *bunnyMesh, true, true, true)));
    DiffuseRenderComponent & bunnyDiffuse = scene.createComponent<DiffuseRenderComponent>(
        scene.renderSystem().getShader<DiffuseShader>()->pid,
        *bunnyMesh,
        ModelTexture(0.3f, glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f)));
    bunny.addComponent(bunnyDiffuse);
    /* Bunny ImGui panes */
    ImGuiComponent & bIc = scene.createComponent<ImGuiComponent>(
        "Bunny", 
        [&]() {
            /* Material properties */
            ImGui::SliderFloat("Ambient", &bunnyDiffuse.modelTexture.material.ambient, 0.f, 1.f);
            ImGui::SliderFloat("Red", &bunnyDiffuse.modelTexture.material.diffuse.r, 0.f, 1.f);
            ImGui::SliderFloat("Green", &bunnyDiffuse.modelTexture.material.diffuse.g, 0.f, 1.f);
            ImGui::SliderFloat("Blue", &bunnyDiffuse.modelTexture.material.diffuse.b, 0.f, 1.f);
            /* Spatial properties */
            // dont want to be setting spat props unnecessarily
            glm::vec3 scale = bunny.getSpatial()->scale();
            if (ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 1.f, 10.f)) {
                bunny.getSpatial()->setScale(scale);
            }
            glm::vec3 position = bunny.getSpatial()->position();

            if (ImGui::SliderFloat3("Position", glm::value_ptr(position), 0.f, 10.f)) {
                bunny.getSpatial()->setPosition(position);
            }
            static glm::vec3 axis; static float angle(0.0f);
            if (
                ImGui::SliderFloat3("Rotation Axis", glm::value_ptr(axis), 0.0f, 1.0f) ||
                ImGui::SliderFloat("Rotation Angle", &angle, -glm::pi<float>(), glm::pi<float>()))
            {
                if (angle != 0.0f && axis != glm::vec3()) {
                    bunny.getSpatial()->setRotation(glm::rotate(angle, glm::normalize(axis)));
                }
                else {
                    bunny.getSpatial()->setRotation(glm::mat3());
                }
            }
        }
    );
    bunny.addComponent(bIc);

    bunny.addComponent(scene.createComponent<PathfindingComponent>(cc, 1.f));

    Mesh * cubeMesh(Loader::getMesh("cube.obj"));
    GameObject & cube(scene.createGameObject());
    cube.addComponent(scene.createComponent<SpatialComponent>(
        glm::vec3(3.f, 0.f, -3.f),
        glm::vec3(1.f, 1.f, 1.f),
        glm::mat3()
    ));
    cube.addComponent(scene.addComponent<BounderComponent>(createBounderFromMesh(INT_MAX, *cubeMesh, true, true, true)));
    cube.addComponent(scene.createComponent<DiffuseRenderComponent>(
        scene.renderSystem().getShader<DiffuseShader>()->pid,
        *cubeMesh,
        ModelTexture(.3f, glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f))));


    /* Game stats pane */
    scene.createComponent<ImGuiComponent>(
        "Stats",
        [&]() {
            ImGui::Text("FPS: %d", engine.fps);
            ImGui::Text("dt: %f", engine.timeStep);
        }
    );

    /* Main loop */
    engine.run();

    scene.shutDown();

    return EXIT_SUCCESS;
}


