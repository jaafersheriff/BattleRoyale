#pragma once



#include "glm/glm.hpp"

#include "System.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



// Singleton
class SpatialSystem : public System {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::spatial;

    public:

    static SpatialSystem & get() {
        static SpatialSystem s_spatialSystem;
        return s_spatialSystem;
    }

    private:

    SpatialSystem() = default;

    public:

    virtual void init() override {}

    virtual void update(float dt) override;

    private:

    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    private:

    std::vector<std::unique_ptr<SpatialComponent>> m_spatialComponents;

};