#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<vector>

class Skybox
{
public:
    Skybox();
    ~Skybox();

    glm::mat4 toWorld;

    void draw(GLuint);
    void loadCubemap(std::vector<const GLchar*> faces);

    // These variables are needed for the shader program
    GLuint VBO, VAO;
    GLuint textureID;
    GLuint uProjection, uModelview;

};

#endif