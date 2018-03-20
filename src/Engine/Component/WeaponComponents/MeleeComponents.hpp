#pragma once



#include "glm/glm.hpp"

#include "Component/Component.hpp"



class BounderComponent;
class SpatialComponent;



class MeleeComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    MeleeComponent(GameObject & gameObject, const SpatialComponent * hostSpatial, const glm::vec3 & offset);

    public:

    MeleeComponent(MeleeComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual void update(float dt) override;

    protected:

    BounderComponent * m_bounder;
    const SpatialComponent * m_hostSpatial;
    glm::vec3 m_hostOffset;

};



class SprayComponent : public MeleeComponent {

    friend Scene;

    protected:

    SprayComponent(GameObject & gameObject, const SpatialComponent * hostSpatial, const glm::vec3 & offset, float damage);

    public:

    SprayComponent(SprayComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual void update(float dt) override;

    protected:

    float m_damage;
    float m_dt;

};