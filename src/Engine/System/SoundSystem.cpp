#include "SoundSystem.hpp"

const Vector<SoundComponent *> & SoundSystem::s_soundComponents(Scene::getComponents<SoundComponent>());
String SoundSystem::s_SOUND_DIR = EngineApp::RESOURCE_DIR + "/soundeffects/";
#ifdef HAVE_FMOD_LIBRARY
FMOD::System* SoundSystem::s_system = NULL;
Map<String, FMOD::Sound*> SoundSystem::s_soundLibrary = Map<String, FMOD::Sound*>();
#endif
void SoundSystem::init() {

#ifdef HAVE_FMOD_LIBRARY
    FMOD_RESULT result;

    result = FMOD::System_Create(&s_system);
    if (result != FMOD_OK)
    {
        printf("failed to create system");
    }

    result = s_system->init(36, FMOD_INIT_NORMAL, NULL);
    if (result != FMOD_OK) {
        printf("failed to initialize system\n");
    }

    initSoundLibrary();
#endif
    
}

void SoundSystem::update(float dt) 
{
#ifdef HAVE_FMOD_LIBRARY
    s_system->update();
#endif
}

#ifdef HAVE_FMOD_LIBRARY
void SoundSystem::initSoundLibrary()
{   
    Vector<String> sfn = getSoundFilenames("filesinfolder.txt");
    for (auto &s : sfn) {
        FMOD::Sound* tempSound = createSound(s);
        s_soundLibrary[s] = tempSound;
    }
}

Vector<String> SoundSystem::getSoundFilenames(String listname) {
    Vector<String> soundfilenames = Vector<String>();
    String line;
    Vector<String> validextensions = {
        ".wav", ".mp3", "aiff", "asf", "asx", "dls", "flac",
        "it", "m3u", "midi", "mod", "mp2", "mp3", "pls",
        "s3m", "wma", "xm"
    };
    String name = s_SOUND_DIR + listname;
    std::ifstream infile(name.c_str());

    while (std::getline(infile, line)) {
        for (auto ext : validextensions) {
            if (line.compare(line.length() - 4, 4, ext) == 0) {
                printf((line + " loaded\n").c_str());
                soundfilenames.push_back(line);
                break;
            }
        }

    }

    return soundfilenames;
}

FMOD::Sound* SoundSystem::createSound(String soundfilename) 
{
    String fullpath = s_SOUND_DIR + soundfilename;
    const char* path = fullpath.c_str();	

    FMOD::Sound *sound;
    FMOD_RESULT result = s_system->createSound(path, FMOD_DEFAULT, 0, &sound);
    if (result != FMOD_OK) {
	    printf("Failed to create sound.\n");
    }

    return sound;
}

//play sound from resources/soundeffects by filename
void  SoundSystem::playSound(String fileName) {
    FMOD::Sound *sound;
    if (s_soundLibrary.count(fileName)) {
        sound = s_soundLibrary[fileName];
    }
    else {
        sound = createSound(fileName);
    }
    FMOD_RESULT result = s_system->playSound(sound, NULL, false, NULL);
    if (result != FMOD_OK) {
        printf("playSound() done goofed!\n");
    }
    
}
#endif