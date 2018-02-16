#include "SoundSystem.hpp"

SoundSystem::SoundSystem(const std::vector<Component *> & components) :
	System(components)
{
#ifdef HAVE_FMOD_LIBRARY
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_system); // Create the Studio System object.
	if (result != FMOD_OK)
	{
		printf("failed to create system");
		exit(-1);
	}

	result = m_system->init(36, FMOD_INIT_NORMAL, NULL);
	if (result != FMOD_OK){
		printf("failed to initialize system\n");
	}
#endif
	SOUND_DIR = "../resources/soundeffects/";
}

void SoundSystem::update(float dt) 
{
#ifdef HAVE_FMOD_LIBRARY
	m_system->update();
#endif
}

#ifdef HAVE_FMOD_LIBRARY
FMOD::Sound* SoundSystem::createSound(std::string soundfilename) 
{
	std::string fullpath = SOUND_DIR + soundfilename;
	const char* path = fullpath.c_str();	

	FMOD::Sound *sound;
	FMOD_RESULT result = m_system->createSound(path, FMOD_DEFAULT, 0, &sound);
	if (result != FMOD_OK) {
		printf("Failed to create sound.\n");
	}

	return sound;
}

void SoundSystem::playSound(int sid) {
	FMOD::Sound *sound = createSound(soundfiles[sid]);
	FMOD_RESULT result = m_system->playSound(sound, NULL, false, NULL);
	if (result != FMOD_OK) {
		printf("playSound() done goofed!\n");
	}
}
#endif

void SoundSystem::setupSoundComponent(SoundComponent *sc)
{
#ifdef HAVE_FMOD_LIBRARY
	sc->sounds.emplace_back(createSound("drill.wav"));
#endif
}