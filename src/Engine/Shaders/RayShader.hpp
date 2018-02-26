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

    void setRay(const Ray & ray); // TODO: don't have it work this way once rendering has been fixed

    private:

    Ray m_ray;
    unsigned int m_vbo, m_ibo, m_vao;
    mutable bool m_rayValid;

};