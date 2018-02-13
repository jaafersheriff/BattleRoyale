#include "Component/Component.hpp"

#include "glm/glm.hpp"



class SpatialSystem;



class SpatialComponent : public Component {

    friend Scene;

    public:

    using SystemClass = SpatialSystem;

    private:

    glm::vec3 m_position;
    glm::mat3 m_rotation;
    glm::vec3 m_scale;
    bool m_transformedFlag; // has the object been spatially modified this tick

    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat3 m_normalMatrix;
    mutable bool m_modelMatrixValid;
    mutable bool m_normalMatrixValid;

    protected: // only scene can create component

    SpatialComponent();
    SpatialComponent(const glm::vec3 & position, const glm::vec3 & scale, const glm::mat3 & rotation);

    public:

    virtual void update(float dt) override;

    // sets the absolute position
    void setPosition(const glm::vec3 & pos);

    // moves current position by delta
    void move(const glm::vec3 & delta);

    // sets the absolute scale
    void setScale(const glm::vec3 & scale);

    // multiplies current scale by factor
    void scale(const glm::vec3 & factor);

    // sets the absolute rotation
    void setRotation(const glm::mat3 & rot);
    
    // rotates current rotation by mat
    void rotate(const glm::mat3 & mat);

    const glm::vec3 & position() const { return m_position; }
    const glm::vec3 & scale() const { return m_scale; }
    const glm::mat3 & rotation() const { return m_rotation; }

    bool transformedFlag() const { return m_transformedFlag; }
    // Collision system needs this. It should be the only thing that calls this.
    // This is not a nice solution, but it's simple and direct
    void clearTransformedFlag() { m_transformedFlag = false; }

    const glm::mat4 & modelMatrix() const;
    const glm::mat3 & normalMatrix() const;

    private:

    void detModelMatrix() const;
    void detNormalMatrix() const;

};