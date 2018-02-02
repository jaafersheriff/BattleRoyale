#include "GameObject.hpp"

GameObject::GameObject() {

}

/* Initialize all components */
void GameObject::init() {
    for (Component *c : components) {
        c->init();
    }
}

// TODO : optimize
template<class T>
T* GameObject::getComponent() {
    T* comp;
    for (auto c : components) {
        if ((comp = dynamic_cast<T *>(c))) {
            return comp;
        }
    }
    return nullptr;
}

/* Add component to list */
void GameObject::addComponent(Component *component) {
    component->setGameObject(this);
    components.push_back(component);
}

/* Take in a message and pass it to all components that match the sender's desired type */
void GameObject::sendMessage(Message *msg) {
    for (auto c : components) {
        // TODO : solve this
        if (c != nullptr && c.type == msge->type) {
            c->receiveMessage(msg);
        }
    }
}
