#include "Particle.h"
using namespace util;
float toRadians = glm::pi<float>() / 180.f;
Particle::Particle( glm::vec3 position, glm::vec3 velocity, float life_length, float scale) {

    this->position = position;
    this->velocity = velocity;
    this->life_length = life_length;
    this->scale = scale;

    this->prev_time = glfwGetTime();
    this->elapsed_time = 0;
}

bool Particle::update() {
    if (elapsed_time < life_length) {
        curr_time = (float)glfwGetTime();
        double frame_time = curr_time - prev_time;
        if (rand() % 80 < 1) {
            velocity = glm::vec3(glm::rotate(glm::mat4(1.f), randomBetween(0, 360) * toRadians, glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(velocity, 1.f));
            velocity = glm::vec3(glm::rotate(glm::mat4(1.f), randomBetween(0, 360) * toRadians, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(velocity, 1.f));
            velocity = glm::vec3(glm::rotate(glm::mat4(1.f), randomBetween(0, 360) * toRadians, glm::vec3(0.f, 0.f, 1.f)) * glm::vec4(velocity, 1.f));
        }
        prev_time = curr_time;

        glm::vec3 change = glm::vec3(velocity);
        change = glm::vec3(glm::scale(glm::mat4(1.f), glm::vec3(frame_time, frame_time, frame_time)) * glm::vec4(change, 1.f));

        position += change;
        // height check
        //if (position.y < 0.f)
            //return false;
        elapsed_time += frame_time;
        return true;

    }
    else {
        return false;
    }

}