#include "Component/Component.hpp"



#include "glm/glm.hpp"



class GameLogicSystem;



class PlayerControllerComponent : public Component {

    friend Scene;
    friend GameLogicSystem;

    protected: // only scene can create component

    PlayerControllerComponent();

    public:

    virtual SystemID systemID() const override { return SystemID::gameLogic; };

    virtual void update(float dt) override;

};