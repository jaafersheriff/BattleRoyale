#pragma once
#define HAVE_FMOD_LIBRARY true
#include "System.hpp"

#ifdef HAVE_FMOD_LIBRARY 
#include "fmod_studio.hpp"
#endif

class SoundSystem : public System {
public:
#ifdef HAVE_FMOD_LIBRARY
	FMOD::Studio::System *system = NULL;
#endif

private:

public:
	SoundSystem(const std::vector<Component *> & comps);

	virtual void update(float dt) override;
};