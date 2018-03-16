#pragma once



#include "ParticleComponent.hpp"
#include "GameObject/Message.hpp"
#include "Scene/Scene.hpp"



// Simply destroys the game object when it has no more particle components
class ParticleAssasinComponent : public Component {

        friend Scene;

    public:

        ParticleAssasinComponent(GameObject & gameObject) :
            Component(gameObject)
        {}

        ParticleAssasinComponent(ParticleAssasinComponent && other) = default;

        virtual void update(float dt) override {
            if (gameObject().getComponentsByType<ParticleComponent>().empty()) {
                Scene::destroyGameObject(gameObject());
            }
        }
    
    public:

        virtual SystemID systemID() const override { return SystemID::particle; }
        
};