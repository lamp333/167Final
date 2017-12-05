#ifndef _NODE_H_
#define _NODE_H_
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Node {
public:
    virtual void draw(GLuint shaderProgram, glm::mat4 C) = 0;
    virtual void update() = 0;
};

#endif