#pragma once



#include "Shader.hpp"



class Component;
class CameraComponent;



class BounderShader : public Shader {

    public:

    BounderShader(const std::string & vertFile, const std::string & fragFile);

    bool init() override;

    virtual void render(const CameraComponent & camera, const std::vector<Component *> &) override;

    private:

    bool initAABMesh();
    bool initSphereMesh();
    bool initCapMesh();
    bool initRodMesh();

    private:

    unsigned int m_aabVBO, m_aabIBO, m_aabVAO;
    unsigned int m_sphereVBO, m_sphereIBO, m_sphereVAO;
    unsigned int m_capVBO, m_capIBO, m_capVAO;
    unsigned int m_rodVBO, m_rodIBO, m_rodVAO;
    int m_nAABIndices, m_nSphereIndices, m_nCapIndices, m_nRodIndices;

};