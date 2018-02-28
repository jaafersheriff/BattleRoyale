#pragma once
#ifndef _SOUND_SYSTEM_HPP_
#define _SOUND_SYSTEM_HPP_

#include "System.hpp"
#include "Component/SoundComponents/SoundComponent.hpp"
#include "Scene/Scene.hpp"

#include "EngineApp/EngineApp.hpp"

#ifdef HAVE_FMOD_LIBRARY 
#include <fmod.hpp>
#include <fmod_studio.hpp>

typedef struct {
    int id;
    FMOD_VECTOR *pos = new FMOD_VECTOR();
    FMOD_VECTOR *vel = new FMOD_VECTOR();
    FMOD_VECTOR *forward = new FMOD_VECTOR();
    FMOD_VECTOR *up = new FMOD_VECTOR();
} Listener;
#endif

class SoundComponent;

class SoundSystem {

    friend Scene;

    public:
        static constexpr SystemID ID = SystemID::sound;

    /* Attributes */
    public:
    #ifdef HAVE_FMOD_LIBRARY
        static FMOD::System* s_system;
    #endif

    private:
        static String s_SOUND_DIR;
        static const Vector<SoundComponent *> & s_soundComponents;
    #ifdef HAVE_FMOD_LIBRARY
        static Listener s_player;
        static Map<String, FMOD::Sound*> s_soundLibrary;
    #endif

    /* Constructor */
    public:
        static void init();
        static void update(float dt);

    #ifdef HAVE_FMOD_LIBRARY
        static void  playSound(String name);
        static void playSound3D(String name, glm::vec3 pos);
    #endif

	private:
        static void added(Component & component) {};
        static void removed(Component & component) {};
    
    #ifdef HAVE_FMOD_LIBRARY
        static Vector<String> getSoundFilenames(String listname);
        static FMOD::Sound* createSound(String soundfilename, FMOD_MODE m);
        static void initSoundLibrary();
        static void updateListener(glm::vec3 pos, glm::vec3 vel, glm::vec3 forward, glm::vec3 up);
        static void setFVec(FMOD_VECTOR *v, glm::vec3 g);
    #endif
};  
#endif


