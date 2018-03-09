#pragma once
#ifndef _SOUND_COMPONENT_HPP_
#define _SOUND_COMPONENT_HPP_

#include "glm/glm.hpp"

#include "Component/Component.hpp"

#ifdef HAVE_FMOD_LIBRARY
#include <fmod.hpp>
#include <fmod_studio.hpp>
#endif

class SoundSystem;

class SoundComponent : public Component{
    
    friend SoundSystem;
    
    public:
        using SystemClass = SoundSystem;

    public:
        SoundComponent();
};

#endif // !_SOUND_COMPONENT_HPP_