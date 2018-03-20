#pragma once



#include "glm/glm.hpp"



class Positionable {

    public:

    Positionable();
    Positionable(const glm::vec3 & position);

    virtual ~Positionable() = default;

    virtual void update();

    virtual void setPosition(const glm::vec3 & position);

    virtual void move(const glm::vec3 & delta);

    virtual const glm::vec3 & position() const { return m_position; }
    virtual const glm::vec3 & prevPosition() const { return m_prevPosition; }
    virtual glm::vec3 position(float interpP) const;

    virtual bool isChange() const { return m_isChange; }

    private:

    glm::vec3 m_position;
    glm::vec3 m_prevPosition;
    bool m_isChange;

};