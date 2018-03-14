#pragma once



#include "glm/glm.hpp"

#include "Component/Component.hpp"



class SphereBounderComponent;



class BlastComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    BlastComponent(GameObject & gameObject, float damage);

    public:

    BlastComponent(BlastComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override;

    protected:

    SphereBounderComponent * m_bounder;
    float m_damage;
    bool m_updated;

};