#include "SoundSystem.hpp"

SoundSystem::SoundSystem(const std::vector<Component *> & components) :
	System(components)
{
#ifdef HAVE_FMOD_LIBRARY
	FMOD_RESULT result;

	result = FMOD::Studio::System::create(&system); // Create the Studio System object.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d)\n", result);
		exit(-1);
	}

	// Initialize FMOD Studio, which will also initialize FMOD Low Level
	result = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK){
		printf("FMOD error! (%d)\n", result);
		exit(-1);
	}
#endif

}

void SoundSystem::update(float dt) 
{
#ifdef HAVE_FMOD_LIBRARY
	system->update();
#endif
}