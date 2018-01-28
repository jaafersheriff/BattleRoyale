#include "Enemy.hpp"

Enemy::Enemy(Mesh *m, ModelTexture mt, const glm::vec3 p, const glm::vec3 r, const glm::vec3 s) :
    Entity(m, mt, p, r, s) {
        this->hit = false;

}

void Enemy::update(std::vector<Entity *> entities) {
    if (!this->hit) {
        if (glm::distance(this->playerPos, this->position) > 3.f) {


            glm::vec3 direction = glm::normalize(this->playerPos - this->position);
            //std::cout << direction.x << " " << direction.y << " " << direction.z << " " << std::endl;
            //std::cout << this->position.x << " " << this->position.y << " " << this->position.z << " " << std::endl;
            float speed = .1f;

            this->position = direction * speed + this->position;

        }
        else {
            this->hit = true;
            this->modelTexture = ModelTexture(.3f, glm::vec3(0.5f, 0.6f, 1.f), glm::vec3(1.f));
        }
    }
}