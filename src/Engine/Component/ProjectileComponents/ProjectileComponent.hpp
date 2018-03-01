#pragma once



#include "Component/Component.hpp"



class BounderComponent;



class ProjectileComponent : public Component {

    friend Scene;

    protected: // only scene or friends can create component

    ProjectileComponent(GameObject & gameObject);

    public:

    ProjectileComponent(ProjectileComponent && other) = default;

    protected:

    virtual void init() override;

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override {};

    private:

    BounderComponent * m_bounder;

};