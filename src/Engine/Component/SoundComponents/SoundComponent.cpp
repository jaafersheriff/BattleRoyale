#include "SoundComponent.hpp"

SoundComponent::SoundComponent()
{
}

#if HAVE_FMOD_LIBRARY
SoundComponent::SoundComponent(FMOD::System *m_system) :
    m_system(m_system)
{
}
#endif
