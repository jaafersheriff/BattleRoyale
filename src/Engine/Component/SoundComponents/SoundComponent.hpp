#pragma once
#ifndef _SOUND_COMPONENT_HPP_
#define _SOUND_COMPONENT_HPP_

#include "glm/glm.hpp"

#include "System/SoundSystem.hpp"

#ifdef HAVE_FMOD_LIBRARY
#include "fmod.hpp"
#include "fmod_studio.hpp"
#endif

#include "Component/Component.hpp"

class SoundSystem;

class SoundComponent : public Component{
	
	friend SoundSystem;

	public:
		/* Attributes*/
		using SystemClass = SoundSystem;

	private:
	#ifdef HAVE_FMOD_LIBRARY
		FMOD::System *m_system;
		
	#endif

	public:

		/* Constructors */
		SoundComponent();
	#ifdef HAVE_FMOD_LIBRARY
		SoundComponent(FMOD::System *m_system);	
	#endif
		/* Functions */
};

#endif // !_SOUND_COMPONENT_HPP_