#include "Component/Component.hpp"



class Scene;
class GameObject;



class AnimationComponent : public Component {

    friend Scene;

    protected:

    AnimationComponent(GameObject & gameObject, SpatialComponent & spatial) :
        Component(gameObject),
        m_spatial(spatial)
    {}

    public:

    AnimationComponent(AnimationComponent && other) = default;

    protected:

    SpatialComponent & m_spatial;

};



class SpinAnimationComponent : public AnimationComponent {

    friend Scene;

    protected:

    SpinAnimationComponent(GameObject & gameObject, SpatialComponent & spatial, const glm::vec3 & axis, float angularSpeed);

    public:

    SpinAnimationComponent(SpinAnimationComponent && other) = default;

    virtual void update(float dt) override;

    protected:

    glm::vec3 m_axis;
    float m_angularSpeed;

};



// More to be added as demanded