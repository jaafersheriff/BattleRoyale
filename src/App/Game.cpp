#include "Game.hpp"

#include "Lighting.hpp"
#include "Player.hpp"
#include "Enemies.hpp"
#include "Weapons.hpp"




void startGame() {

    //--------------------------------------------------------------------------
    // Message Handling

    // Fire projectile (click)
    // Remove all projectiles (right click)
    auto fireCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && !(msg.mods & GLFW_MOD_CONTROL) && msg.action == GLFW_PRESS) {
            weapons::grenade::firePlayer();
        }
        else if (msg.button == GLFW_MOUSE_BUTTON_2 && msg.action == GLFW_PRESS) {
            weapons::destroyAllProjectiles();
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, fireCallback);

    // Spawn enemy (1)
    auto spawnEnemy([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_1 && msg.action == GLFW_PRESS) {
            enemies::basic::spawn();
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, spawnEnemy);

    // Shoot ray (ctrl-click)
    int rayDepth(100);
    Vector<glm::vec3> rayPositions;
    auto rayPickCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && msg.mods & GLFW_MOD_CONTROL && msg.action == GLFW_PRESS) {
            rayPositions.clear();
            rayPositions.push_back(player::spatial->position());
            glm::vec3 dir(player::camera->getLookDir());
            for (int i(0); i < rayDepth; ++i) {
                auto pair(CollisionSystem::pick(Ray(rayPositions.back() + dir * 0.001f, dir)));
                if (!pair.second.is) {
                    break;
                }
                rayPositions.push_back(pair.second.pos);
                dir = glm::normalize(glm::reflect(dir, pair.second.face ? pair.second.norm : -pair.second.norm));
            }
            RenderSystem::getShader<RayShader>()->setPositions(rayPositions);
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, rayPickCallback);

    // Toggle Freecam (ctrl-tab)
    auto camSwitchCallback([&](const Message & msg_) {
        static bool free = false;

        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_TAB && msg.action == GLFW_PRESS && msg.mods & GLFW_MOD_CONTROL) {
            if (free) {
                // disable camera controller
                freecam::controllerComp->setEnabled(false);
                // enable player controller
                player::controller->setEnabled(true);
                RenderSystem::setCamera(player::camera);
            }
            else {
                // disable player controller
                player::controller->setEnabled(false);
                // enable camera object
                freecam::controllerComp->setEnabled(true);
                // set camera object camera to player camera
                freecam::spatialComp->setPosition(player::spatial->position());
                freecam::spatialComp->setUVW(player::spatial->u(), player::spatial->v(), player::spatial->w());
                freecam::cameraComp->lookInDir(player::camera->getLookDir());
                RenderSystem::setCamera(freecam::cameraComp);
            }
            free = !free;
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, camSwitchCallback);

    // Toggle gravity (ctrl-g), flip gravity (alt-g)
    auto gravCallback([&](const Message & msg_) {
        static glm::vec3 s_gravity = k_defGravity;

        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_G && msg.action == GLFW_PRESS && msg.mods == GLFW_MOD_CONTROL) {
            if (Util::isZero(SpatialSystem::gravity())) {
                SpatialSystem::setGravity(s_gravity);
            }
            else {
                s_gravity = SpatialSystem::gravity();
                SpatialSystem::setGravity(glm::vec3());
            }
        }
        else if (msg.key == GLFW_KEY_G && msg.action == GLFW_PRESS && msg.mods == GLFW_MOD_ALT) {
            s_gravity *= -1.0f;
            SpatialSystem::setGravity(-SpatialSystem::gravity());
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, gravCallback);

    // Destroy game object looking at (delete)
    auto deleteCallback([&] (const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_DELETE && msg.action == GLFW_PRESS) {
            auto pair(CollisionSystem::pick(Ray(player::spatial->position(), player::camera->getLookDir())));
            if (pair.first) Scene::destroyGameObject(const_cast<GameObject &>(pair.first->gameObject()));
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, deleteCallback);

    //--------------------------------------------------------------------------

    // Main loop
    EngineApp::run();
}