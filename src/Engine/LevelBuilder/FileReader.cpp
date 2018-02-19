#include "FileReader.hpp"

#include "System/RenderSystem.hpp"
#include "System/CollisionSystem.hpp"

int FileReader::loadLevel(const char & filePath) {
    rapidjson::Document document;
    glm::vec3 position, scale;
    glm::mat3 rotation;
    

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
        const rapidjson::Value& gameObject = document[m.name.GetString()];

        //Get full filepath of object file
        assert(gameObject["objName"].IsString());
        std::string filePath = gameObject["objName"].GetString();
        std::size_t fileOffset = filePath.find_last_of("/\\");
        filePath = filePath.substr(fileOffset + 1);

        //Get full filepath of texture file
        assert(gameObject["objTexture"].IsString());
        std::string texturePath = gameObject["objTexture"].GetString();
        std::size_t textureOffset = texturePath.find_last_of("/\\");
        texturePath = texturePath.substr(textureOffset + 1);

        //Get position convert to vec3
        const rapidjson::Value& objPosition = gameObject["position"];
        assert(gameObject["position"].IsArray());
        position = glm::vec3(objPosition[0].GetFloat(), objPosition[1].GetFloat(), objPosition[2].GetFloat());

        //Get scale and convert to vec3
        const rapidjson::Value& objScale = gameObject["scale"];
        assert(objScale.IsArray());
        scale = glm::vec3(objScale[0].GetFloat(), objScale [1].GetFloat(), objScale[2].GetFloat());

        //Get rotation convert to mat3
        const rapidjson::Value& objRotation_row0 = gameObject["rotMat3_0"];
        assert(objRotation_row0.IsArray());

        const rapidjson::Value& objRotation_row1 = gameObject["rotMat3_1"];
        assert(objRotation_row1.IsArray());

        const rapidjson::Value& objRotation_row2 = gameObject["rotMat3_2"];
        assert(objRotation_row2.IsArray());

        rotation[0] = glm::vec3(objRotation_row0[0].GetFloat(), objRotation_row0[1].GetFloat(), objRotation_row0[2].GetFloat());
        rotation[1] = glm::vec3(objRotation_row1[0].GetFloat(), objRotation_row1[1].GetFloat(), objRotation_row1[2].GetFloat());
        rotation[2] = glm::vec3(objRotation_row2[0].GetFloat(), objRotation_row2[1].GetFloat(), objRotation_row2[2].GetFloat());

        initGameObject(filePath, texturePath, position, scale, rotation);
    }

    return 0;
}

void FileReader::initGameObject(std::string filePath, std::string texturePath, glm::vec3 position, glm::vec3 scale, glm::mat3 rotation) {

    GameObject & gameObject(Scene::get().createGameObject());
    SpatialComponent & spatComp(Scene::get().addComponent<SpatialComponent>(
        gameObject,
        position, // position
        scale, // scale
        rotation // rotation
    ));
    
    DiffuseRenderComponent & renderComp(Scene::get().addComponent<DiffuseRenderComponent>(
        gameObject,
        RenderSystem::get().getShader<DiffuseShader>()->pid,
        *Loader::getMesh(filePath),
        ModelTexture(Loader::getTexture(texturePath), 0.4f, glm::vec3(0.f), glm::vec3(0.f))
    ));

    BounderComponent & boundComp(CollisionSystem::get().addBounderFromMesh(gameObject, UINT_MAX, *Loader::getMesh(filePath), true, true, true));
        
}