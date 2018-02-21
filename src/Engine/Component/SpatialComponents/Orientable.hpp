#pragma once



#include "glm/glm.hpp"
#include "Util/Util.hpp"



class Orientable {

    public:

    Orientable() :
        m_u(1.0f, 0.0f, 0.0f),
        m_v(0.0f, 1.0f, 0.0f),
        m_w(0.0f, 0.0f, 1.0f),
        m_orientMatrix(),
        m_orientMatrixValid(true)
    {}

    Orientable(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) :
        m_u(u),
        m_v(v),
        m_w(w),
        m_orientMatrix(),
        m_orientMatrixValid(false)
    {}

    virtual ~Orientable() = default;

    virtual void rotate(const glm::mat3 & mat) {
        m_orientMatrix = mat * m_orientMatrix;
        m_orientMatrixValid = true;
        detUVW();
    }

    virtual void setOrientation(const glm::mat3 & orient) {
        m_orientMatrix = orient;
        m_orientMatrixValid = true;
        detUVW();
    }

    virtual void setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) {
        m_u = u;
        m_v = v;
        m_w = w;
        m_orientMatrixValid = false;
    }

    virtual const glm::vec3 & u() const { return m_u; }
    virtual const glm::vec3 & v() const { return m_v; }
    virtual const glm::vec3 & w() const { return m_w; }

    virtual const glm::mat3 & orientationMatrix() const {
        if (!m_orientMatrixValid) detOrientMatrix();
        return m_orientMatrix;
    }

    private:

    void detOrientMatrix() const {
        m_orientMatrix = Util::mapFrom(m_u, m_v, m_w);
        m_orientMatrixValid = true;
    }

    void detUVW() {
        glm::mat3 trans(glm::transpose(m_orientMatrix));
        m_u = trans[0];
        m_v = trans[1];
        m_w = trans[2];
    }

    private:

    glm::vec3 m_u, m_v, m_w;
    
    mutable glm::mat3 m_orientMatrix;
    mutable bool m_orientMatrixValid;

};