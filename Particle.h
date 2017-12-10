#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#include "util.h"

using namespace util;

class Particle {
private:

public:
    glm::vec3 position;
    glm::vec3 velocity;

    double life_length;
    float scale;

    double prev_time;
    double curr_time;
    double elapsed_time;

    Particle(glm::vec3 position, glm::vec3 velocity,
        float life_length, float scale);

    bool update();

};