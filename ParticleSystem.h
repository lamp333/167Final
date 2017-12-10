#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"
#include <vector>
#include <map>

#include "Window.h"
#include "Particle.h"




class ParticleSystem {
private:

    GLuint VAO, VBO_vert, VBO_models, id;
    int maxParticles;
    std::vector<Particle> particles;
    std::vector<glm::mat4> models;


public:
    ParticleSystem(std::string path, int p);
    ~ParticleSystem();
    void init();
    void bindData();
    void setTexture(std::string path);

    void refreshModel(glm::vec3 pos, float scale);
    glm::mat4 unproject(glm::mat4 model);
    void render(GLuint shaderProgram);

    void update();
    void addParticle(int x, int y, int z);
    void generate(float delta, int x, int y, int z);
};