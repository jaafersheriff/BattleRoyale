#pragma once



#include "glm/glm.hpp"

#include "System.hpp"
#include "Component/PostCollisionComponents/GroundComponent.hpp"



// Singleton
class PostCollisionSystem : public System {

    friend Scene;

    public:

    static constexpr SystemID ID = SystemID::postCollision;

    public:

    static PostCollisionSystem & get() {
        static PostCollisionSystem s_postCollisionSystem;
        return s_postCollisionSystem;
    }

    private:

    PostCollisionSystem() = default;

    public:

    virtual void init() override {};

    virtual void update(float dt) override;

    private:

    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    private:

    std::vector<std::unique_ptr<GroundComponent>> m_groundComponents;

};