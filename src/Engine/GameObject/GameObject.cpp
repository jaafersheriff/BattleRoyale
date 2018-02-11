#include "GameObject.hpp"
#include "Component/Component.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"

GameObject::GameObject() :
    components(),
    spatialComponent(nullptr)
{}

GameObject::~GameObject() {
    // TODO 
}

/* Initialize all components */
void GameObject::init() {
    for (Component *c : components) {
        c->init();
    }
}

/* Add component to list */
void GameObject::addComponent(Component & component) {
    component.setGameObject(this);
    components.push_back(&component);

    if (dynamic_cast<SpatialComponent *>(&component)) {
        spatialComponent = static_cast<SpatialComponent *>(&component);
    }
}

/* Take in a message and pass it to all components that match the sender's 
 * desired type */
void GameObject::sendMessage(Message & msg) {
    for (Component *c : components) {
        // TODO : how does message know which component to send to?
        // if (c != nullptr && c->type == msg->type) {
        //     c->receiveMessage(msg);
        // }
    }
}
