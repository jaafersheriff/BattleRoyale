#pragma once



#include "Shader.hpp"
#include "Util/Geometry.hpp"



class Component;
class CameraComponent;



class RayShader : public Shader {

    public:

    RayShader(const String & vertFile, const String & fragFile);

    bool init() override;

    virtual void render(const CameraComponent * camera, const Vector<Component *> &) override;

    void setPositions(const Vector<glm::vec3> & positions);

    private:

    Vector<glm::vec3> m_positions;
    unsigned int m_vbo, m_ibo, m_vao;
    mutable bool m_positionsValid;

};