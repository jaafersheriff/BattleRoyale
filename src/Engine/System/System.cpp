#include "System.hpp"



void System::add(std::unique_ptr<Component> component) {
    m_initQueue.emplace_back(std::move(component));
}