#include "SoundSystem.hpp"

const Vector<SoundComponent *> & SoundSystem::s_soundComponents(Scene::getComponents<SoundComponent>());
String SoundSystem::SOUND_DIR = "../resources/soundeffects/";
Vector<String> SoundSystem::soundfiles = {
    "drill.wav",
    "doorbump.wav",
    "softbump.wav"
};
FMOD::System* SoundSystem::m_system = NULL;
Map<String, FMOD::Sound*> SoundSystem::soundLibrary = Map<String, FMOD::Sound*>();
void SoundSystem::init() {

#if HAVE_FMOD_LIBRARY
    FMOD_RESULT result;

    result = FMOD::System_Create(&m_system);
    if (result != FMOD_OK)
    {
        printf("failed to create system");
    }

    result = m_system->init(36, FMOD_INIT_NORMAL, NULL);
    if (result != FMOD_OK) {
        printf("failed to initialize system\n");
    }

    initSoundLibrary();
#endif
    playSound("drill.wav");
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

FMOD::Sound* SoundSystem::createSound(String soundfilename) 
{
    String fullpath = SOUND_DIR + soundfilename;
    const char* path = fullpath.c_str();	

    FMOD::Sound *sound;
    FMOD_RESULT result = m_system->createSound(path, FMOD_DEFAULT, 0, &sound);
    if (result != FMOD_OK) {
	    printf("Failed to create sound.\n");
    }

    return sound;
}

//play sound from resources/soundeffects by filename
void  SoundSystem::playSound(String fileName) {
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