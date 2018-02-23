#pragma once
#ifndef _SOUND_SYSTEM_HPP_
#define _SOUND_SYSTEM_HPP_

// Set whether you have fmod library or not. Must be done manually!
#ifndef HAVE_FMOD_LIBRARY
#define HAVE_FMOD_LIBRARY true
#endif

#include "System.hpp"
#include "Component/SoundComponents/SoundComponent.hpp"
#include "Scene/Scene.hpp"
#include <string>
#include <vector>
#include <map>

#if HAVE_FMOD_LIBRARY 
#include "ThirdParty/fmod/fmod.hpp"
#include "ThirdParty/fmod/fmod_studio.hpp"
#endif

class SoundComponent;

class SoundSystem {

    /* Attributes */
    public:
        static std::vector<std::string> soundfiles;
    #if HAVE_FMOD_LIBRARY
        static FMOD::System *m_system; // = NULL;
    #endif

    private:
        static std::string SOUND_DIR;
    #if HAVE_FMOD_LIBRARY
        static std::map<std::string, FMOD::Sound*> soundLibrary;
    #endif

    /* Constructor */
    public:
        static void init();

    /* Functions */
        static void update(float dt);

    #if HAVE_FMOD_LIBRARY
        
        static void  playSound(std::string name);
    #endif

	private:
    static const std::vector<SoundComponent *> & s_SoundComponents;
    #if HAVE_FMOD_LIBRARY
        static FMOD::Sound* createSound(std::string soundfilename);
        static void initSoundLibrary();
    #endif
};
#endif