#pragma once



#include "Shader.hpp"



class CameraComponent;
class DiffuseRenderComponent;



class BounderShader : public Shader {

    public:

    BounderShader(const String & vertFile, const String & fragFile);

    bool init() override;

    virtual void render(const CameraComponent * camera) override;

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