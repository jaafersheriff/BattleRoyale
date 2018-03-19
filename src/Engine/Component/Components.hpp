#pragma once

#include "Component.hpp"
// game
#include "CameraComponents/CameraComponent.hpp"
#include "CameraComponents/CameraControllerComponent.hpp"
#include "ImGuiComponents/ImGuiComponent.hpp"
#include "PlayerComponents/PlayerComponent.hpp"
#include "PlayerComponents/PlayerControllerComponent.hpp"
#include "EnemyComponents/EnemyComponent.hpp"
#include "WeaponComponents/ProjectileComponents.hpp"
#include "WeaponComponents/BlastComponent.hpp"
#include "StatComponents/StatComponents.hpp"
// pathfinding
#include "PathfindingComponents/PathfindingComponent.hpp"
// generate a graph from the map
#include "MapExploreComponents/MapExploreComponent.hpp"
// spatial
#include "SpatialComponents/SpatialComponent.hpp"
#include "SpatialComponents/PhysicsComponents.hpp"
// collision
#include "CollisionComponents/BounderComponent.hpp"
// post collision
#include "PostCollisionComponents/GroundComponent.hpp"
// render
#include "RenderComponents/DiffuseRenderComponent.hpp"
// particle
#include "ParticleComponents/ParticleComponent.hpp"