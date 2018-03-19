#pragma once



// This is to allow other areas of the engine to access the "game" code without
// the need to include the massive and everchanging GameSystem.hpp



class GameObject;



struct GameInterface {

    static float getAmbience();

    static GameObject & getPlayer();

};