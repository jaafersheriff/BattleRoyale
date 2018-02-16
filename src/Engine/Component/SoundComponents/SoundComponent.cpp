#include "SoundComponent.hpp"

SoundComponent::SoundComponent()
{
}

#ifdef HAVE_FMOD_LIBRARY
SoundComponent::SoundComponent(FMOD::System *m_system) :
	m_system(m_system)
{
}
#endif
