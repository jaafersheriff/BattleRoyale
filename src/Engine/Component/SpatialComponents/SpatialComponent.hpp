#pragma once

#include "Component/Component.hpp"

#include "glm/glm.hpp"

#include "Orientable.hpp"



class SpatialSystem;



class SpatialComponent : public Component, public Orientable {

    friend Scene;
    friend SpatialSystem;

    protected: // only scene can create component

    SpatialComponent();
    SpatialComponent(const glm::vec3 & position, const glm::vec3 & scale);
    SpatialComponent(const glm::vec3 & position, const glm::vec3 & scale, const glm::mat3 & orientation);

    public:

    virtual SystemID systemID() const override { return SystemID::spatial; };

    public:

    virtual void init() override;

    virtual void update(float dt) override;

    // sets the absolute position
    void setPosition(const glm::vec3 & pos, bool silently = false);

    // moves current position by delta
    void move(const glm::vec3 & delta, bool silently = false);

    // sets the absolute scale
    void setScale(const glm::vec3 & scale, bool silently = false);

    // multiplies current scale by factor
    void scale(const glm::vec3 & factor, bool silently = false);

    // sets the absolute rotation
    void setOrientation(const glm::mat3 & rot, bool silently = false);
    
    // rotates current rotation by mat
    void rotate(const glm::mat3 & mat, bool silently = false);

    // set the orthonormal basis vectors
    void setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w, bool silently = false);

    public:

    const glm::vec3 & position() const { return m_position; }
    const glm::vec3 & scale() const { return m_scale; }

    const glm::mat4 & modelMatrix() const;
    const glm::mat3 & normalMatrix() const;

    private:

    void detModelMatrix() const;
    void detNormalMatrix() const;

    private:

    glm::vec3 m_position;
    glm::vec3 m_scale;

    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat3 m_normalMatrix;
    mutable bool m_modelMatrixValid;
    mutable bool m_normalMatrixValid;

};