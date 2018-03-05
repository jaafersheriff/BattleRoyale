#include "SoundSystem.hpp"

const Vector<SoundComponent *> & SoundSystem::s_soundComponents(Scene::getComponents<SoundComponent>());
String SoundSystem::s_SOUND_DIR = EngineApp::RESOURCE_DIR + "/soundeffects/";

#ifdef HAVE_FMOD_LIBRARY
FMOD::System* SoundSystem::s_system = NULL;
Map<String, FMOD::Sound*> SoundSystem::s_soundLibrary = Map<String, FMOD::Sound*>();
CameraComponent* SoundSystem::s_camera = NULL;
Sound* SoundSystem::s_bgMusic = NULL;
#endif

void SoundSystem::init() {
#ifdef HAVE_FMOD_LIBRARY
    FMOD_RESULT result;

    result = FMOD::System_Create(&s_system);
    if (result != FMOD_OK)
    {
        printf("failed to create system");
    }

    result = s_system->init(36, FMOD_INIT_3D_RIGHTHANDED, NULL);
    if (result != FMOD_OK) {
        printf("failed to initialize system\n");
    }

    float dopplescale = 1.0f;
    float distancefactor = 1.0f;
    float rolloffscale = 1.0f;
    result = s_system->set3DSettings(dopplescale, distancefactor, rolloffscale);
    if (result != FMOD_OK) {
        printf("failed to set up 3D settings\n");
    }

    initSoundLibrary();

#endif
    
}

void SoundSystem::update(float dt) 
{
#ifdef HAVE_FMOD_LIBRARY
    updateListener();
    s_system->update();
#endif
}

void SoundSystem::setCamera(CameraComponent *camera) {
    s_camera = camera;
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

void SoundSystem::setBackgroundMusic(String name, bool loop) {
    FMOD::Sound* bgSound = NULL;
    if (loop) {
        bgSound = createSound(name, FMOD_LOOP_NORMAL);
    }
    else {
        bgSound = createSound(name, FMOD_DEFAULT);
    }
    if (bgSound != NULL) {
        s_bgMusic = new Sound();
        s_bgMusic->sound = bgSound;
        s_bgMusic->loop = loop;
    }
}

void SoundSystem::playBackgroundMusic() {
    if (s_bgMusic != NULL) {
        FMOD::Channel *channel = NULL;
        FMOD_RESULT result = s_system->playSound(s_bgMusic->sound, NULL, false, &channel);
        s_bgMusic->channel = channel;
        if (result != FMOD_OK) {
            printf("playBackgroundMusic() done goofed!\n");
        }
    }
}

void SoundSystem::pauseBackgroundMusic() {
    if (s_bgMusic != NULL && s_bgMusic->channel != NULL) {
        s_bgMusic->channel->setPaused(true);
    }
}

void SoundSystem::unpauseBackgroundMusic() {
    if (s_bgMusic != NULL && s_bgMusic->channel != NULL) {
        s_bgMusic->channel->setPaused(false);
    }
}

void SoundSystem::setBackGroundLoop(bool loop) {
    if (s_bgMusic != NULL) {
        if (loop && !s_bgMusic) {
            s_bgMusic->loop = true;
            if (s_bgMusic->channel != NULL) {
                s_bgMusic->channel->setPaused(true);
                s_bgMusic->sound->setMode(FMOD_LOOP_NORMAL);
                s_bgMusic->channel->setMode(FMOD_LOOP_NORMAL);
                s_bgMusic->channel->setPaused(false);
            }
            else {
                s_bgMusic->sound->setMode(FMOD_LOOP_NORMAL);
            }
        }
        else if (!loop && s_bgMusic) {
            s_bgMusic->loop = false;
            if (s_bgMusic->channel != NULL) {
                s_bgMusic->channel->setPaused(true);
                s_bgMusic->sound->setMode(FMOD_DEFAULT);
                s_bgMusic->channel->setMode(FMOD_DEFAULT);
                s_bgMusic->channel->setPaused(false);
            }
            else {
                s_bgMusic->sound->setMode(FMOD_DEFAULT);
            }
        }
    }
}

void SoundSystem::setBackgroundMusicVolume(float volume) {
    if (s_bgMusic != NULL && s_bgMusic->channel != NULL) {
        s_bgMusic->channel->setVolume(volume);
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
                #ifdef DEBUG_MODE
                    printf((line + " loaded\n").c_str());
                #endif
                soundfilenames.push_back(line);
                break;  
            }
        }

    }

    return soundfilenames;
}

void SoundSystem::updateListener() {
    if (s_camera != NULL) {
        SpatialComponent* s = s_camera->gameObject().getSpatial();
        s_system->set3DListenerAttributes(0,
            fVec(s->position()), 
            NULL, 
            fVec(s_camera->getLookDir()), 
            fVec(s_camera->v())
        );
    }
}

FMOD_VECTOR* SoundSystem::fVec(glm::vec3 v) {
    FMOD_VECTOR *fv =  new FMOD_VECTOR();
    fv->x = v.x;
    fv->y = v.y;
    fv->z = v.z;
    return fv;
}

FMOD::Sound* SoundSystem::createSound(String soundfilename, FMOD_MODE m) 
{
    String fullpath = s_SOUND_DIR + soundfilename;
    const char* path = fullpath.c_str();	

    FMOD::Sound *sound = NULL;
    FMOD_RESULT result = s_system->createSound(path, m, 0, &sound);
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

    if (s_soundLibrary.count(fileName + "3D")) {
        sound = s_soundLibrary[fileName + "3D"];
    }   
    else {
        sound = createSound(fileName + "3D", FMOD_3D);
    }

    FMOD_RESULT result = s_system->playSound(sound, NULL, true, &newChannel);
    newChannel->set3DAttributes(fVec(pos), NULL, NULL);
    newChannel->setPaused(false);
    if (result != FMOD_OK) {
        printf("playSound() done goofed!\n");
    }
}
#endif