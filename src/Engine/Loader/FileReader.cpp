#include "FileReader.hpp"

#include "System/RenderSystem.hpp"
#include "System/CollisionSystem.hpp"

void FileReader::addSpatialComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform) {
    glm::vec3 position, scale;
    glm::mat3 rotation;

    //Get position convert to vec3
    const rapidjson::Value& objPosition = jsonTransform["position"];
    assert(jsonTransform["position"].IsArray());
    position = glm::vec3(objPosition[0].GetFloat(), objPosition[1].GetFloat(), objPosition[2].GetFloat());

    //Get scale and convert to vec3
    const rapidjson::Value& objScale = jsonTransform["scale"];
    assert(objScale.IsArray());
    scale = glm::vec3(objScale[0].GetFloat(), objScale[1].GetFloat(), objScale[2].GetFloat());

    //Get rotation convert to mat3
    const rapidjson::Value& objRotation_row0 = jsonTransform["rotMat3_0"];
    assert(objRotation_row0.IsArray());

    const rapidjson::Value& objRotation_row1 = jsonTransform["rotMat3_1"];
    assert(objRotation_row1.IsArray());

    const rapidjson::Value& objRotation_row2 = jsonTransform["rotMat3_2"];
    assert(objRotation_row2.IsArray());

    rotation[0] = glm::vec3(objRotation_row0[0].GetFloat(), objRotation_row0[1].GetFloat(), objRotation_row0[2].GetFloat());
    rotation[1] = glm::vec3(objRotation_row1[0].GetFloat(), objRotation_row1[1].GetFloat(), objRotation_row1[2].GetFloat());
    rotation[2] = glm::vec3(objRotation_row2[0].GetFloat(), objRotation_row2[1].GetFloat(), objRotation_row2[2].GetFloat());

    Scene::addComponent<SpatialComponent>(
        gameObject,
        position, // position
        scale, // scale
        rotation // rotation
        );
}

void FileReader::addRenderComponent(GameObject & gameObject, const rapidjson::Value& jsonTransform, const String filePath, float ambience) {

    //Get full filepath of texture file
    assert(jsonTransform["objTexture"].IsString());
    String texturePath = jsonTransform["objTexture"].GetString();
    std::size_t textureOffset = texturePath.find_last_of("/\\");
    texturePath = texturePath.substr(textureOffset + 1);

    //Get isToon bool
    const rapidjson::Value& isToon = jsonTransform["isToon"];
    assert(isToon.IsBool());

    const rapidjson::Value& jsonTiling = jsonTransform["tiling"];
    glm::vec2 tiling = glm::vec2(jsonTiling[0].GetFloat(), jsonTiling[1].GetFloat());

    //Add diffuse component
    DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(
        gameObject,
        RenderSystem::getShader<DiffuseShader>()->pid,
        *Loader::getMesh(filePath),
        ModelTexture(Loader::getTexture(texturePath, GL_REPEAT, true), ambience, glm::vec3(1.0f), glm::vec3(1.0f)),
        isToon.GetBool(), tiling);
}

int FileReader::addCapsuleColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject) {
    int numberOfColliders = 0;
    const rapidjson::Value& jsonCapsules = jsonObject["capsules"];

    for (auto& c : jsonCapsules.GetObject()) {
        const rapidjson::Value& jsonCap = jsonCapsules[c.name.GetString()];

        const rapidjson::Value& centerV = jsonCap["center"];
        const rapidjson::Value& radiusV = jsonCap["radius"];
        const rapidjson::Value& heightV = jsonCap["height"];

        glm::vec3 center(centerV[0].GetFloat(), centerV[1].GetFloat(), centerV[2].GetFloat());
        float radius(radiusV.GetFloat());
        float height(heightV.GetFloat());
        Capsule gameObjectCap(center, radius, glm::max(0.0f, (height - 2 * radius)));
        Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(gameObject, UINT_MAX, gameObjectCap);

        numberOfColliders++;
    }
    return numberOfColliders;
}

int FileReader::addSphereColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject) {
    int numberOfColliders = 0;
    const rapidjson::Value& jsonSpheres = jsonObject["spheres"];

    for (auto& c : jsonSpheres.GetObject()) {
        const rapidjson::Value& jsonSphere = jsonSpheres[c.name.GetString()];

        const rapidjson::Value& centerV = jsonSphere["center"];
        const rapidjson::Value& radiusV = jsonSphere["radius"];

        glm::vec3 center(centerV[0].GetFloat(), centerV[1].GetFloat(), centerV[2].GetFloat());
        float radius(radiusV.GetFloat());
        Sphere gameObjectSphere(center, radius);
        Scene::addComponentAs<SphereBounderComponent, BounderComponent>(gameObject, UINT_MAX, gameObjectSphere);

        numberOfColliders++;
    }
    return numberOfColliders;
}

int FileReader::addBoxColliderComponents(GameObject & gameObject, const rapidjson::Value& jsonObject) {
    int numberOfColliders = 0;
    const rapidjson::Value& jsonBoxes = jsonObject["boxes"];

    for (auto& c : jsonBoxes.GetObject()) {
        const rapidjson::Value& jsonBox = jsonBoxes[c.name.GetString()];

        const rapidjson::Value& minV = jsonBox["min"];
        const rapidjson::Value& maxV = jsonBox["max"];

        glm::vec3 min(minV[0].GetFloat(), minV[1].GetFloat(), minV[2].GetFloat());
        glm::vec3 max(maxV[0].GetFloat(), maxV[1].GetFloat(), maxV[2].GetFloat());
        AABox gameObjectBox(min, max);
        Scene::addComponentAs<AABBounderComponent, BounderComponent>(gameObject, UINT_MAX, gameObjectBox);

        numberOfColliders++;
    }
    return numberOfColliders;
}

int FileReader::loadLevel(const char & filePath, float ambience) {
    rapidjson::Document document;
    int numberOfColliders;

    //Open and read json file
    FILE* fp = fopen(&filePath, "rb");
    char readBuffer[65536];
    rapidjson::FileReadStream fs(fp, readBuffer, sizeof(readBuffer));

    if (document.ParseStream(fs).HasParseError()) {
        std::cout << "bad json file" << std::endl;
        return 1;
    }

    //For each object in the json document
    for (auto& m : document.GetObject()) {
        const rapidjson::Value& jsonObject = document[m.name.GetString()];
        const rapidjson::Value& jsonTransform = jsonObject["transform"];
        GameObject & gameObject(Scene::createGameObject());
      
        numberOfColliders = 0;

        //Parse filepath
        assert(jsonTransform["objName"].IsString());
        String filePath = jsonTransform["objName"].GetString();
        std::size_t fileOffset = filePath.find_last_of("/\\");
        filePath = filePath.substr(fileOffset + 1);

        //Read the transform data from the json
        FileReader::addSpatialComponent(gameObject, jsonTransform);
        
        //Read the collider data from the json
        numberOfColliders += FileReader::addCapsuleColliderComponents(gameObject, jsonObject);
        numberOfColliders += FileReader::addSphereColliderComponents(gameObject, jsonObject);
        numberOfColliders += FileReader::addBoxColliderComponents(gameObject, jsonObject);

        const rapidjson::Value& allowColliders = jsonTransform["allowColliders"];
        //Create a bounder if none have been created the the json allows colliders on the mesh
        if (numberOfColliders == 0 && allowColliders.GetBool()) {
            CollisionSystem::addBounderFromMesh(gameObject, UINT_MAX, *Loader::getMesh(filePath), true, true, true);
        }

        //Read the texture data from the json
        if(filePath.compare("") != 0)
            FileReader::addRenderComponent(gameObject, jsonTransform, filePath, ambience);
    }

    return 0;
}