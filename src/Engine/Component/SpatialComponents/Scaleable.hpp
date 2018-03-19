#pragma once



#include "glm/glm.hpp"



class Scaleable {

    public:

    Scaleable();
    Scaleable(const glm::vec3 & scale);

    virtual ~Scaleable() = default;

    virtual void update();

    virtual void setScale(const glm::vec3 & scale);

    virtual void scale(const glm::vec3 & factor);

    virtual const glm::vec3 & scale() const { return m_scale; }
    virtual const glm::vec3 & prevScale() const { return m_prevScale; }
    virtual glm::vec3 scale(float interpP) const;

    virtual bool isChange() const { return m_isChange; }

    private:

    glm::vec3 m_scale;
    glm::vec3 m_prevScale;
    bool m_isChange;

};