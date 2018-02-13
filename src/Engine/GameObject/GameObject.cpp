#include "GameObject.hpp"
#include "Component/Component.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/Systems.hpp"

GameObject::GameObject() :
    m_allComponents(),
    m_components(),
    m_spatialComponent(nullptr)
{}

GameObject::~GameObject() {

}

/* Initialize all components */
void GameObject::init() {
    for (Component * c : m_allComponents) {
        c->init();
    }
}

/* Take in a message and pass it to all components that match the sender's 
 * desired type */
void GameObject::sendMessage(Message & msg) {
    for (Component * c : m_allComponents) {
        // TODO : how does message know which component to send to?
        // if (c != nullptr && c->type == msg->type) {
        //     c->receiveMessage(msg);
        // }
    }
}
