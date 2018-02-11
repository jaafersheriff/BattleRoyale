#pragma once

#include "Component/Component.hpp"
#include "../CameraComponents/CameraComponent.hpp"

#include "glm/glm.hpp"
#include <iostream>

class PathfindingSystem;

class PathfindingComponent : public Component {
	
	public:

		using SystemClass = PathfindingSystem;

		PathfindingComponent(CameraComponent & cc, float ms) :
			target(&cc),
			moveSpeed(ms)
		{}


		void init();
		void update(float);

	private:
		CameraComponent *target;
		float moveSpeed;
};