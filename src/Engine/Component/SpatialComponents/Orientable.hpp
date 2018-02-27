#pragma once



#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "Util/Util.hpp"



class Orientable {

    public:

    Orientable() :
        m_u(1.0f, 0.0f, 0.0f),
        m_v(0.0f, 1.0f, 0.0f),
        m_w(0.0f, 0.0f, 1.0f),
        m_orientation(),
        m_prevOrientation(),
        m_orientMatrix(),
        m_isChange(false)
    {}

    Orientable(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) :
        m_u(u),
        m_v(v),
        m_w(w),
        m_orientation(),
        m_prevOrientation(),
        m_orientMatrix(),
        m_isChange(false)
    {
        detOrientMatrixFromUVW();
        m_orientation = glm::toQuat(m_orientMatrix);
        m_prevOrientation = m_orientation;
    }

    virtual ~Orientable() = default;

    virtual void setOrientation(const glm::quat & orient) {
        m_orientation = orient;
        m_prevOrientation = m_orientation;
        m_orientMatrix = glm::toMat3(m_orientation);
        detUVWFromOrientMatrix();
        m_isChange = false;
    }

    virtual void setOrientation(const glm::mat3 & orient) {
        m_orientMatrix = orient;
        m_orientation = glm::toQuat(m_orientMatrix);
        m_prevOrientation = m_orientation;
        detUVWFromOrientMatrix();
        m_isChange = false;
    }

    virtual void rotate(const glm::quat & rot) {
        m_prevOrientation = m_orientation;
        m_orientation = rot * m_orientation;
        m_orientMatrix = glm::toMat3(m_orientation);
        detUVWFromOrientMatrix();
        m_isChange = m_orientation != m_prevOrientation;
    }

    virtual void rotate(const glm::mat3 & rot) {
        m_prevOrientation = m_orientation;
        m_orientMatrix = m_orientMatrix * rot;
        m_orientation = glm::toQuat(m_orientMatrix);
        detUVWFromOrientMatrix();
        m_isChange = m_orientation != m_prevOrientation;
    }

    virtual void setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w) {
        m_u = u;
        m_v = v;
        m_w = w;
        detOrientMatrixFromUVW();
        m_orientation = glm::toQuat(m_orientMatrix);
        m_prevOrientation = m_orientation;
        m_isChange = false;
    }

    virtual const glm::vec3 & u() const { return m_u; }
    virtual const glm::vec3 & v() const { return m_v; }
    virtual const glm::vec3 & w() const { return m_w; }

    virtual const glm::quat & orientation() const {
        return m_orientation;
    }

    virtual glm::quat orientation(float interpP) const {
        if (!m_isChange) {
            return m_orientation;
        }
        // TODO: make sure lerp is good enough for our purposes. Could use
        // slerp, but it uses 4 trig functions, which is hella expensive
        return glm::lerp(m_prevOrientation, m_orientation, interpP);
    }

    virtual const glm::mat3 & orientMatrix() const {
        return m_orientMatrix;
    }

    virtual glm::mat3 orientMatrix(float interpP) const {
        if (!m_isChange) {
            return m_orientMatrix;
        }
        return glm::toMat3(orientation(interpP));
    }

    private:

    void detOrientMatrixFromUVW() {
        m_orientMatrix = Util::mapFrom(m_u, m_v, m_w);
    }

    void detUVWFromOrientMatrix() {
        glm::mat3 trans(glm::transpose(m_orientMatrix));
        m_u = trans[0];
        m_v = trans[1];
        m_w = trans[2];
    }

    private:

    glm::vec3 m_u, m_v, m_w;    
    glm::quat m_orientation;
    glm::quat m_prevOrientation;
    glm::mat3 m_orientMatrix;
    bool m_isChange;

};