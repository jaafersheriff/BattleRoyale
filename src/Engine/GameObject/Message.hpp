/* Message struct used to communicate data between components */
#pragma once
#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_



#include "glm/glm.hpp"



class GameObject;



struct Message {
    
    GameObject * gameObject;

    Message(GameObject * gameObject) : gameObject(gameObject) {}

};



struct LocationSetMessage : public Message {

    glm::vec3 location;

};



class Receiver {

    public:

    virtual void receive(Message & msg) = 0;

};



#endif