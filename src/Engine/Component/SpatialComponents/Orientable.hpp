#pragma once



#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"



class Orientable {

    public:

    Orientable();
    Orientable(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w);
    Orientable(const glm::quat & orient);
    Orientable(const glm::mat3 & orient);

    virtual ~Orientable() = default;

    virtual void update();

    virtual void setOrientation(const glm::quat & orient);
    virtual void setOrientation(const glm::mat3 & orient);

    virtual void rotate(const glm::quat & rot);
    virtual void rotate(const glm::mat3 & rot);

    virtual void setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w);

    virtual const glm::vec3 & u() const { return m_orientMatrix[0]; }
    virtual const glm::vec3 & v() const { return m_orientMatrix[1]; }
    virtual const glm::vec3 & w() const { return m_orientMatrix[2]; }
    virtual const glm::vec3 & prevU() const { return m_prevOrientMatrix[0]; }
    virtual const glm::vec3 & prevV() const { return m_prevOrientMatrix[1]; }
    virtual const glm::vec3 & prevW() const { return m_prevOrientMatrix[2]; }

    virtual const glm::quat & orientation() const { return m_orientation; }
    virtual const glm::quat & prevOrientation() const { return m_prevOrientation; }
    virtual glm::quat orientation(float interpP) const;

    virtual const glm::mat3 & orientMatrix() const { return m_orientMatrix; }
    virtual const glm::mat3 & prevOrientMatrix() const { return m_prevOrientMatrix; }
    virtual glm::mat3 orientMatrix(float interpP) const;

    virtual bool isChange() const { return m_isChange; }

    private:
  
    glm::quat m_orientation;
    glm::quat m_prevOrientation;
    glm::mat3 m_orientMatrix;
    glm::mat3 m_prevOrientMatrix;
    bool m_isChange;

};