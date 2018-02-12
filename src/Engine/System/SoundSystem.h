#pragma once

# include "System.hpp"

class SoundSystem : public System {
public:

	constexpr static Type type = SOUND;

public:
	SoundSystem(const std::vector<Component *> & comps);
};