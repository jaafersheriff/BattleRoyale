#pragma once

#include "System.hpp"
#include "Component/EnemyComponents/EnemyComponent.hpp"

// static class
class EnemySystem {
    friend Scene;
    
    public:

    static constexpr SystemID ID = SystemID::enemy;

    public:

    static void init() {};

    static void update(float dt);

    private:

    static const Vector<EnemyComponent *> & s_enemyComponents;
};