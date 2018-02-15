#pragma once
#ifndef _GAME_LOGIC_SYSTEM_HPP_
#define _GAME_LOGIC_SYSTEM_HPP_



#include "System.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/CameraComponents/CameraController.hpp"
#include "Component/ImGuiComponents/ImGuiComponent.hpp"



// Singleton
class GameLogicSystem : public System {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::gameLogic;

    public:

    static GameLogicSystem & get() {
        static GameLogicSystem s_gameLogicSystem;
        return s_gameLogicSystem;
    }

    private:

    GameLogicSystem() = default;

    public:

    virtual void init() override {};

    virtual void update(float dt) override;

    private:
    
    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    private:

    std::vector<std::unique_ptr<CameraComponent>> m_cameraComponents;
    std::vector<std::unique_ptr<CameraController>> m_cameraControllers;
    std::vector<std::unique_ptr<ImGuiComponent>> m_imguiComponents;

};

#endif