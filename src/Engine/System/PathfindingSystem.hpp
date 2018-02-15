#pragma once

#include "System.hpp"
#include "Component/PathfindingComponents/PathfindingComponent.hpp"

class PathfindingSystem : public System {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::spatial;

    public:

    static PathfindingSystem & get() {
        static PathfindingSystem s_pathfindingSystem;
        return s_pathfindingSystem;
    }

    private:

    PathfindingSystem() = default;
    
    public:

    virtual void init() override {}

    virtual void update(float dt) override;

    private:

    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    private:

    std::vector<std::unique_ptr<PathfindingComponent>> m_pathfindingComponents;

};