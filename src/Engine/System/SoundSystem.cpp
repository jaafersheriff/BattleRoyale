#include "SoundSystem.hpp"

const Vector<SoundComponent *> & SoundSystem::s_soundComponents(Scene::getComponents<SoundComponent>());
String SoundSystem::s_SOUND_DIR = EngineApp::RESOURCE_DIR + "/soundeffects/";
#ifdef HAVE_FMOD_LIBRARY
FMOD::System* SoundSystem::s_system = NULL;
Map<String, FMOD::Sound*> SoundSystem::s_soundLibrary = Map<String, FMOD::Sound*>();
Listener SoundSystem::s_player = Listener();
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
        FMOD::Sound* tempSound = createSound(s, FMOD_DEFAULT);
        s_soundLibrary[s + "2D"] = tempSound;
        tempSound = createSound(s, FMOD_3D);
        s_soundLibrary[s + "3D"] = tempSound;
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

void SoundSystem::updateListener(glm::vec3 pos, glm::vec3 vel, glm::vec3 forward, glm::vec3 up) {
    s_player.id = 0;
    setFVec(s_player.pos, pos);
    setFVec(s_player.vel, vel);
    setFVec(s_player.forward, forward);
    setFVec(s_player.up, up);
}

void SoundSystem::setFVec(FMOD_VECTOR *v, glm::vec3 g) {
    v->x = g.x;
    v->y = g.y;
    v->z = g.z;
}

FMOD::Sound* SoundSystem::createSound(String soundfilename, FMOD_MODE m) 
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
    if (s_soundLibrary.count(fileName + "2D")) {
        sound = s_soundLibrary[fileName + "2D"];
    }
    else {
        sound = createSound(fileName + "2D", FMOD_DEFAULT);
    }
    FMOD_RESULT result = s_system->playSound(sound, NULL, false, NULL);
    if (result != FMOD_OK) {
        printf("playSound() done goofed!\n");
    }
}

void SoundSystem::playSound3D(String fileName, glm::vec3 pos) {
    FMOD::Sound *sound;
    FMOD::Channel *newChannel;
    FMOD_VECTOR fPos;

    if (s_soundLibrary.count(fileName + "3D")) {
        sound = s_soundLibrary[fileName + "3D"];
    }
    else {
        sound = createSound(fileName + "3D", FMOD_3D);
    }

    setFVec(&fPos, pos);
    FMOD_RESULT result = s_system->playSound(sound, NULL, false, &newChannel);
    newChannel->setPaused(true);
    newChannel->set3DAttributes(&fPos, NULL, NULL);
    newChannel->setPaused(false);
    if (result != FMOD_OK) {
        printf("playSound() done goofed!\n");
    }
}
#endif