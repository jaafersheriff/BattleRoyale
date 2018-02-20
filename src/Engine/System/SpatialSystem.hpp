#pragma once



#include "glm/glm.hpp"

#include "System.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"



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

    virtual void init() override {};

    virtual void update(float dt) override;

    void setGravity(const glm::vec3 & gravity);
    void setGravityDir(const glm::vec3 & dir);
    void setGravityMag(float mag);

    glm::vec3 gravity() const { return m_gravityDir * m_gravityMag; }
    const glm::vec3 & gravityDir() const { return m_gravityDir; }
    float gravityMag() const { return m_gravityMag; }
    float coefficientOfFriction() const { return m_coefficientOfFriction; }

    private:

    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    private:

    std::vector<std::unique_ptr<SpatialComponent>> m_spatialComponents;
    std::vector<std::unique_ptr<NewtonianComponent>> m_newtonianComponents;
    std::vector<std::unique_ptr<AcceleratorComponent>> m_acceleratorComponents;
    glm::vec3 m_gravityDir = glm::vec3(0.0f, -1.0f, 0.0f);
    float m_gravityMag = 10.0f;
    float m_coefficientOfFriction = 0.1f;

};