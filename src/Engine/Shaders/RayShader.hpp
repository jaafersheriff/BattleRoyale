#pragma once



#include "Shader.hpp"



class CameraComponent;
class DiffuseRenderComponent;



class RayShader : public Shader {

    public:

    RayShader(const String & vertFile, const String & fragFile);

    bool init() override;

    virtual void render(const CameraComponent * camera) override;

    void setPositions(const Vector<glm::vec3> & positions);

    private:

    Vector<glm::vec3> m_positions;
    unsigned int m_vbo, m_ibo, m_vao;
    mutable bool m_positionsValid;

};