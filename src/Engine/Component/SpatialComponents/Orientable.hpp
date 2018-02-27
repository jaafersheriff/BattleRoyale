#pragma once



#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"



class Orientable {

    public:

    Orientable();
    Orientable(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w);

    virtual ~Orientable() = default;

    virtual void update();

    virtual void setOrientation(const glm::quat & orient);
    virtual void setOrientation(const glm::mat3 & orient);

    virtual void rotate(const glm::quat & rot);
    virtual void rotate(const glm::mat3 & rot);

    virtual void setUVW(const glm::vec3 & u, const glm::vec3 & v, const glm::vec3 & w);

    const glm::vec3 & u() const { return m_u; }
    const glm::vec3 & v() const { return m_v; }
    const glm::vec3 & w() const { return m_w; }

    const glm::quat & orientation() const { return m_orientation; }
    glm::quat orientation(float interpP) const;

    const glm::mat3 & orientMatrix() const { return m_orientMatrix; }
    glm::mat3 orientMatrix(float interpP) const;

    bool isChange() const { return m_isChange; }

    private:

    void detOrientMatrixFromUVW();
    void detUVWFromOrientMatrix();

    private:

    glm::vec3 m_u, m_v, m_w;    
    glm::quat m_orientation;
    glm::quat m_prevOrientation;
    glm::mat3 m_orientMatrix;
    bool m_isChange;

};