#include "CameraComponent.hpp"
#include "Util/Util.hpp"

#include <glm/gtc/matrix_transform.hpp>

void CameraComponent::init() {
    this->projection = glm::perspective(fov, aspect, near, far);
    update(0.f);
}

void CameraComponent::update(float dt) {
    /* Update basis vectors */
    w = glm::normalize(lookAt - gameObject->transform.position);
    u = glm::normalize(glm::cross(w, glm::vec3(0, 1, 0)));
    v = glm::normalize(glm::cross(u, w));

    /* Update look at */
    glm::vec3 sphere(
        glm::cos(phi)*glm::cos(theta),
        glm::sin(phi),
        glm::cos(phi)*glm::cos((Util::PI / 2.f) - theta));
    lookAt = gameObject->transform.position + glm::normalize(sphere);

    /* Update view matrix */
    this->projection = glm::perspective(fov, aspect, near, far);
    this->view = glm::lookAt(gameObject->transform.position, lookAt, glm::vec3(0, 1, 0));
}