#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Util/Util.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "IO/Window.hpp"

void CameraComponent::init() {
    this->projection = glm::perspective(fov, Window::getAspectRatio(), near, far);
    this->view = glm::lookAt(gameObject->getSpatial()->position(), lookAt, glm::vec3(0, 1, 0));
    phi = theta = 0.0;
    update(0.f);
}

void CameraComponent::update(float dt) {
    /* Update basis vectors */
    w = glm::normalize(lookAt - gameObject->getSpatial()->position());
    u = glm::normalize(glm::cross(w, glm::vec3(0, 1, 0)));
    v = glm::normalize(glm::cross(u, w));

    /* Update look at */
    glm::vec3 sphere(
        glm::cos(phi)*glm::cos(theta),
        glm::sin(phi),
        glm::cos(phi)*glm::cos((Util::PI / 2.f) - theta));
    lookAt = gameObject->getSpatial()->position() + glm::normalize(sphere);

    /* Update view matrix */
    if (isDirty) {
        this->projection = glm::perspective(fov, Window::getAspectRatio(), near, far);
        this->view = glm::lookAt(gameObject->getSpatial()->position(), lookAt, glm::vec3(0, 1, 0));
        isDirty = false;
    }
}
