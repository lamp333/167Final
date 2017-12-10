#include "ParticleSystem.h"
#include "util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "packages\stb_image.h"

using namespace util;
GLfloat VERTICES[12] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f
};

ParticleSystem::ParticleSystem(std::string path, int p) {
    maxParticles = p;
    setTexture(path);
    init();
}

ParticleSystem::~ParticleSystem() {
    //delete(texture.tex);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_vert);
    glDeleteBuffers(1, &VBO_models);
}

void ParticleSystem::setTexture(std::string path) {


    int width, height;
    int nrChannels;
    unsigned char* image;
    image = stbi_load(path.c_str(), &width,&height,&nrChannels, STBI_rgb_alpha);
    //Generate a texture
    glGenTextures(1, &id);

    //Set all data for GL_TEXTURE_2D to be sent to the generated texture
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    free(image);

    //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}


void ParticleSystem::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_vert);
    glGenBuffers(1, &VBO_models);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), &VERTICES, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    

    glBindBuffer(GL_ARRAY_BUFFER, VBO_models);
    glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleSystem::bindData() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_models);
    glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::refreshModel(glm::vec3 pos, float scale) {
    //mat4 matrix from position
    glm::mat4 modelMat = glm::translate(glm::mat4(1.f), pos);

    modelMat = unproject(modelMat);
    //apply rotations and scale
    modelMat = glm::scale(glm::mat4(1.f), glm::vec3(scale, scale, scale)) * modelMat;
    models.push_back(modelMat);
}

glm::mat4 ParticleSystem::unproject(glm::mat4 model) {
    glm::mat4 unprojected = model;
    unprojected[0][0] = Window::V[0][0];
    unprojected[0][1] = Window::V[1][0];
    unprojected[0][2] = Window::V[2][0];
    unprojected[1][0] = Window::V[0][1];
    unprojected[1][1] = Window::V[1][1];
    unprojected[1][2] = Window::V[2][1];
    unprojected[2][0] = Window::V[0][2];
    unprojected[2][1] = Window::V[1][2];
    unprojected[2][2] = Window::V[2][2];

    return unprojected;
}

void ParticleSystem::render(GLuint shaderProgram) {
    update();
    models.clear();
    for (int i = 0; i < particles.size(); i++) {
        refreshModel(particles[i].position, particles[i].scale);
    }
    glBindVertexArray(VAO);

    bindData();
    

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &Window::V[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());

    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthMask(true);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glBindVertexArray(0);
}

void ParticleSystem::update() {
    printf("%d\n", particles.size());
    auto it = particles.begin();
    int i = 0;
    while (it != particles.end()) {
        if (it->update()) {
            it++;
        }
        else {
            it = particles.erase(it);
        }
    }
}

void ParticleSystem::addParticle(int x, int y, int z) {

    float xPos = randomBetween(0, x / 2);
    if (rand() % 2 < 1) {
        xPos = -xPos;
    }
    float yPos = randomBetween(0, y / 2);
    if (rand() % 2 < 1) {
        yPos = -yPos;
    }
    float zPos = randomBetween(0, z / 2);
    if (rand() % 2 < 1) {
        zPos = -zPos;
    }

    glm::vec3 to_push = glm::vec3(xPos, yPos, zPos);
    particles.push_back(Particle(to_push, glm::vec3(3, 3, 3), 10, 0.3f));

}

void ParticleSystem::generate(float delta, int x, int y, int z) {
    int newparticles = (int)(delta*100.0);
    if (newparticles > (int)(0.016f*100.0))
        newparticles = (int)(0.016f*100.0);

    for (int i = 0; i<newparticles; i++) {
        if (particles.size() < maxParticles)
            addParticle(x, y, z);
    }
}
