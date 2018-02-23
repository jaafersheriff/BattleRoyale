#pragma once
#ifndef _SOUND_COMPONENT_HPP_
#define _SOUND_COMPONENT_HPP_

#include "glm/glm.hpp"

#include "Component/Component.hpp"

#ifndef HAVE_FMOD_LIBRARY
#define HAVE_FMOD_LIBRARY false
#endif

#if HAVE_FMOD_LIBRARY
#include "ThirdParty/fmod/fmod.hpp"
#include "ThirdParty/fmod/fmod_studio.hpp"
#endif

class SoundSystem;

class SoundComponent : public Component{
    
    friend SoundSystem;
    
    public:
        /* Attributes*/
        using SystemClass = SoundSystem;

    private:
    #if HAVE_FMOD_LIBRARY
        FMOD::System *m_system;
    #endif

    public:

        /* Constructors */
        SoundComponent();
    #if HAVE_FMOD_LIBRARY
	    SoundComponent(FMOD::System *m_system);	
    #endif
};

#endif // !_SOUND_COMPONENT_HPP_