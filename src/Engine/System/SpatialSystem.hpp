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
    
    // sets the absolute position
    void setObjPosition(GameObject & go, const glm::vec3 & position);

    // moves current position by delta
    void moveObj(GameObject & go, const glm::vec3 & delta);

    // sets the absolute scale
    void setObjScale(GameObject & go, const glm::vec3 & scale);

    // multiplies current scale by factor
    void scaleObj(GameObject & go, const glm::vec3 & factor);

    // sets the absolute rotation
    void setObjRotation(GameObject & go, const glm::mat3 & rot);
    
    // rotates current rotation by mat
    void rotateObj(GameObject & go, const glm::mat3 & mat);

    private:

    virtual void add(std::unique_ptr<Component> component) override;

    virtual void remove(Component * component) override;

    private:

    std::vector<std::unique_ptr<SpatialComponent>> m_spatialComponents;

};