#include "glm/glm.hpp"

#include "Component/Component.hpp"



class Scene;
class SpatialSystem;
class SpatialComponent;



class NewtonianComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    NewtonianComponent(SpatialComponent & spatial, float maxSpeed);

    public:

    virtual SystemID systemID() const override { return SystemID::spatial; };

    public:

    virtual ~NewtonianComponent() = default;

    virtual void init() override;

    virtual void update(float dt) override;

    void accelerate(const glm::vec3 & acceleration);

    const glm::vec3 & velocity() const { return m_velocity; }

    protected:

    SpatialComponent & m_spatial;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    float m_maxSpeed;

};



class AcceleratorComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    AcceleratorComponent(NewtonianComponent & newtonian, const glm::vec3 & acceleration);

    public:

    virtual SystemID systemID() const override { return SystemID::spatial; };

    public:

    virtual ~AcceleratorComponent() = default;

    virtual void update(float dt) override;

    protected:

    NewtonianComponent & m_newtonian;
    glm::vec3 m_acceleration;

};



class GravityComponent : public AcceleratorComponent {

    friend Scene;

    protected: // only scene or friends can create component

    GravityComponent(NewtonianComponent & newtonian);

    public:

    virtual ~GravityComponent() = default;

    void setDirection(const glm::vec3 & dir);

};
