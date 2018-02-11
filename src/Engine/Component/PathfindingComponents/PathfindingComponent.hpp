#pragma once

#include "Component/Component.hpp"
#include "../CameraComponents/CameraComponent.hpp"

#include "glm/glm.hpp"
#include <iostream>

class PathfindingSystem;

class Pathfinding : public Component {
	
	public:

		using SystemClass = PathfindingSystem;

		Pathfinding(CameraComponent & cc, float ms) :
			target(&cc),
			moveSpeed(ms)
		{}


		void init();
		void update(float);

	private:
		CameraComponent *target;
		float moveSpeed;
};