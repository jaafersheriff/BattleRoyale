#include "SoundSystem.hpp"

SoundSystem::SoundSystem(const std::vector<Component *> & components) :
    System(components)
{
    SOUND_DIR = "../resources/soundeffects/";
#if HAVE_FMOD_LIBRARY
    FMOD_RESULT result;
    
    result = FMOD::System_Create(&m_system);
    if (result != FMOD_OK)
    {
        printf("failed to create system");
    }
    
    result = m_system->init(36, FMOD_INIT_NORMAL, NULL);
    if (result != FMOD_OK){
    	printf("failed to initialize system\n");
    }

    initSoundLibrary();
#endif
}

void SoundSystem::update(float dt) 
{
#if HAVE_FMOD_LIBRARY
    m_system->update();
#endif
}

#if HAVE_FMOD_LIBRARY
void SoundSystem::initSoundLibrary()
{
    for (auto s : soundfiles) {
        FMOD::Sound* tempSound = createSound(s);
        soundLibrary[s] = tempSound;
    }
}

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

//play sound from resources/soundeffects by filename
void SoundSystem::playSound(std::string fileName) {
    FMOD::Sound *sound;
    if (soundLibrary.count(fileName)) {
        sound = soundLibrary[fileName];
    }
    else {
        sound = createSound(fileName);
    }
    FMOD_RESULT result = m_system->playSound(sound, NULL, false, NULL);
    if (result != FMOD_OK) {
        printf("playSound() done goofed!\n");
    }
}
#endif