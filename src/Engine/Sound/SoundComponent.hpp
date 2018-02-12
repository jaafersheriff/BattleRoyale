#pragma once
#ifndef _SOUNDCOMPONENT_HPP_
#define _SOUNDCOMPONENT_HPP_

#include "glm/glm.hpp"
#include "ThirdParty\fmod\api\lowlevel\inc\fmod.h"
#include "System\SoundSystem.h"
#include "Component\Component.hpp"

class SoundSystem;


class SoundComponent : public Component{
	public:
		/* Attributes*/
		using SystemClass = SoundSystem;

	private:
		FILE * fp;
		double m_duration;
		double m_current;

	public:

		/* Constructors */
		SoundComponent();

		/* Functions */
		bool playSound();
};

#endif // !_SOUNDCOMPONENT_HPP_