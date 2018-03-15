#pragma once
#ifndef _SOUND_SYSTEM_HPP_
#define _SOUND_SYSTEM_HPP_

#include "System.hpp"
#include "Component/SoundComponents/SoundComponent.hpp"
#include "Component/CameraComponents/CameraComponent.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Scene/Scene.hpp"

#include "EngineApp/EngineApp.hpp"

#ifdef HAVE_FMOD_LIBRARY 
#include <fmod.hpp>
#include <fmod_studio.hpp>

typedef struct {
    FMOD::Sound* sound = NULL;
    bool loop = false;
    FMOD::Channel* channel = NULL;
} Sound;
#endif

class SoundComponent;

class SoundSystem {

    friend Scene;

    public:
        static constexpr SystemID ID = SystemID::sound;

    public:
    #ifdef HAVE_FMOD_LIBRARY
        static FMOD::System* s_system;
    #endif

    private:
        static String s_SOUND_DIR;
        static const Vector<SoundComponent *> & s_soundComponents;
        static CameraComponent* s_camera;
    #ifdef HAVE_FMOD_LIBRARY

        static Sound* s_bgMusic;
        static Map<String, FMOD::Sound*> s_soundLibrary;
    #endif

    public:
        static void init();
        static void update(float dt);

        static void setCamera(CameraComponent *camera);

    #ifdef HAVE_FMOD_LIBRARY
        static void playSound(String name);
        static void playSound(String name, bool loop);
        static void playSound3D(String name, glm::vec3 pos);
        static void playSound3D(String name, glm::vec3 pos, bool loop);
        static void setBackgroundMusic(String name, bool loop);
        static void pauseBackgroundMusic();
        static void unpauseBackgroundMusic();
        static void playBackgroundMusic();
        static void setBackgroundMusicVolume(float volume);
        static void setBackGroundLoop(bool loop);
    #endif

	private:
        static void added(Component & component) {};
        static void removed(Component & component) {};
    
    #ifdef HAVE_FMOD_LIBRARY
        static Vector<String> getSoundFilenames(String listname);
        static FMOD::Sound* createSound(String soundfilename, FMOD_MODE m);
        static void initSoundLibrary();
        static void updateListener();
        static FMOD_VECTOR* fVec(glm::vec3 v);
    #endif
};  
#endif


