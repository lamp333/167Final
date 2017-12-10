#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
namespace util {

    unsigned char* loadPPM(const GLchar* filename, int &width, int &height);
    float randomBetween(int min, int max);
}