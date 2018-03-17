/* Message struct used to communicate data between components */
#pragma once
#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_



#include "glm/glm.hpp"
// Don't add includes. If possible, forward declard. This file shouldn't contain
// any functionality, and it will be included all over the place.



//==============================================================================
// README !!!
//------------------------------------------------------------------------------ 
// A message can be sent from anywhere using Scene's sendMessage method, which
// looks like...
//
//     Scene::sendMessage<MessageType>([nullptr | gameobject], message args);
//
// This message is sent out to all receivers of the specified message type. If
// gameobject is not null, the message is ALSO sent to any receiver of that
// game object and message type. This is for efficient inter-component
// communication.
//
// The scene keeps a list of messages, and what type they were, until it's time
// to relay the messages. This happens before and after every system update.
// The system will only relay messages to any receivers of the corresponding
// type. A receiver is a std::function<void (const Message &)> . To add a
// receiver, do...
//
//     Scene::addReceiver<MessageType>([nullptr | gameobject], receiver);
//
// Here, if gameobject is null, the receiver will receive all messages of the
// specified type. If gameobject is not null, the receiver will only receive
// messages of the specified type that have been sent to that object. This is
// how you do efficient inter-component communication.
//
// For receiver, can pass either a function pointer or a lambda. Note, you can
// technically bind a method to a std::function, but it's ugly. For adding
// receivers from objects that can reference that object, I reccommend using a
// lambda, like so...
//    
//    auto receiver = [&](const Message & msg_) {
//        const MessageIWantType & msg(static_cast<const MessageIWantType &>(msg_));
//        ...
//    };
//    Scene::addReceiver<MessageIWantType>(receiver);
//
// There is also the idea of a Tag, which is simply a way for messages of
// different types but sharing the same data to be accessed. For an example of
// this, take a look in CollisionSystem's init method. Tags must not be used as
// message types in sendMessage or addReceiver! They are only helpers.
//------------------------------------------------------------------------------



class GameObject;
class SpatialComponent;
class BounderComponent;
class CameraComponent;
class PlayerComponent;



struct Message {
    virtual ~Message() = default;
};



// a game object was initialized. This is sent out after Scene's init queue is finished
struct ObjectInitMessage : public Message {
    ObjectInitMessage() {}
};



// a spatial was transformed in some way
// tag for the following six messages
// Don't use this as a message type!!
struct SpatialTransformTag : public Message {
    const SpatialComponent & spatial;
    SpatialTransformTag(const SpatialComponent & spatial) : spatial(spatial) {}
};

// a spatial's location was set
struct SpatialPositionSetMessage :  public SpatialTransformTag {
    SpatialPositionSetMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial was moved
struct SpatialMovedMessage :  public SpatialTransformTag {
    SpatialMovedMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial's scale was set
struct SpatialScaleSetMessage :  public SpatialTransformTag {
    SpatialScaleSetMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial was scaled
struct SpatialScaledMessage :  public SpatialTransformTag {
    SpatialScaledMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial's rotation was set
struct SpatialOrientationSetMessage :  public SpatialTransformTag {
    SpatialOrientationSetMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial was rotated
struct SpatialRotatedMessage :  public SpatialTransformTag {
    SpatialRotatedMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};



// a camera was rotated
struct CameraRotatedMessage : public Message {
    const CameraComponent & camera;
    CameraRotatedMessage(const CameraComponent & camera) : camera(camera) {}
};



// a collision occurred between the two bounders
// there may not necessarily have been any adjustment
// two messages are sent per pair of bounders, where the order of bounders is swapped
struct CollisionMessage : public Message {
    const BounderComponent & bounder1, & bounder2;
    CollisionMessage(const BounderComponent & bounder1, const BounderComponent & bounder2) : bounder1(bounder1), bounder2(bounder2) {}
};

// a collision occured where norm is the normal of the surface collided with
// mainly for physics
struct CollisionNormMessage : public Message {
    const BounderComponent & bounder;
    glm::vec3 norm; // normalized
    CollisionNormMessage(const BounderComponent & bounder, const glm::vec3 & norm) : bounder(bounder), norm(norm) {}
};

// a collision occurred and the game object's position was adjusted
struct CollisionAdjustMessage : public Message {
    const GameObject & gameObject;
    glm::vec3 delta;
    CollisionAdjustMessage(const GameObject & gameObject, const glm::vec3 & delta) : gameObject(gameObject), delta(delta) {}
};



// a bouncy newtonian bounced
struct BounceMessage : public Message {
    glm::vec3 norm;
    BounceMessage(const glm::vec3 & norm) : norm(norm) {}
};



// the window was resized
struct WindowFrameSizeMessage : public Message {
    glm::ivec2 frameSize;
    WindowFrameSizeMessage(const glm::ivec2 & frameSize) : frameSize(frameSize) {}
};



// key input
struct KeyMessage : public Message {
    int key, action, mods;
    KeyMessage(int key, int action, int mods) : key(key), action(action), mods(mods) {}
};

// mouse input
struct MouseMessage : public Message {
    int button, action, mods;
    MouseMessage(int button, int action, int mods) : button(button), action(action), mods(mods) {}
};

// scroll input
struct ScrollMessage : public Message {
    float dx, dy;
    ScrollMessage(float dx, float dy) : dx(dx), dy(dy) {}
};



// component has put through the init queue and added to the scene
struct ComponentAddedMessage : public Message {
    Component & comp;
    std::type_index typeI;
    ComponentAddedMessage(Component & comp, std::type_index typeI) : comp(comp), typeI(typeI) {}
};

// component has been added to kill queue and will be removed from the scene
struct ComponentRemovedMessage : public Message {
    UniquePtr<Component> comp;
    std::type_index typeI;
    ComponentRemovedMessage(UniquePtr<Component> && comp, std::type_index typeI) : comp(std::move(comp)), typeI(typeI) {}
};



// The player's health has fallen to 0
struct PlayerDeadMessage : public Message {
    const PlayerComponent & player;
    PlayerDeadMessage(const PlayerComponent & player) : player(player) {}
};



#endif