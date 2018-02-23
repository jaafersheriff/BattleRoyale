#pragma once
#ifndef _SOUND_SYSTEM_HPP_
#define _SOUND_SYSTEM_HPP_

// Set whether you have fmod library or not. Must be done manually!
/*#ifndef HAVE_FMOD_LIBRARY
#define HAVE_FMOD_LIBRARY true
#endif*/

#include "System.hpp"
#include "Component/SoundComponents/SoundComponent.hpp"
#include "Scene/Scene.hpp"
//#include <string>
//#include <vector>
//#include <map>

#ifdef HAVE_FMOD_LIBRARY 
#include "ThirdParty/fmod/fmod.hpp"
#include "ThirdParty/fmod/fmod_studio.hpp"
#endif

class SoundComponent;

class SoundSystem {

    friend Scene;

    public:
        static constexpr SystemID ID = SystemID::sound;

    /* Attributes */
    public:
        static Vector<String> soundfiles;
    #ifdef HAVE_FMOD_LIBRARY
        static FMOD::System* m_system; // = NULL;
    #endif

    private:
        static String SOUND_DIR;
        static const Vector<SoundComponent *> & s_soundComponents;
    #ifdef HAVE_FMOD_LIBRARY
        static Map<String, FMOD::Sound*> soundLibrary;
    #endif

    /* Constructor */
    public:
        static void init();

    /* Functions */
        static void update(float dt);

    #ifdef HAVE_FMOD_LIBRARY
        
        static void  playSound(String name);
    #endif

	private:
        static void added(Component & component) {};
        static void removed(Component & component) {};
    
    #ifdef HAVE_FMOD_LIBRARY
        static FMOD::Sound* createSound(String soundfilename);
        static void initSoundLibrary();
    #endif
};  
#endif


