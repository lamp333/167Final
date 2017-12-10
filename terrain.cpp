#include "terrain.h"
#include "util.h"
#include "Window.h"
#include <time.h> 

using namespace util;
Terrain::Terrain(int x, int z, float amp) {
    width = x;
    length = z;
    amplitude = amp;

    heightMap = new float*[x];
    for (int i = 0; i < x; i++) {
        heightMap[i] = new float[z];
    }
    setup();

    randomGenerate();

}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

float Terrain::calculateHeight(int x, int z) {
    float inputX = (float) x/16.f;
    float inputZ = (float) z/16.f;
    return interpolateNoise(inputX, inputZ) * amplitude;
}

float Terrain::getNoise(int x, int z) {
    uint32_t seed = uint32_t(x) * 1087;
    seed ^= 0xE56FAA12;
    seed += uint32_t(z) * 2749;
    seed ^= 0x69628a2d;
    seed += uint32_t(x) * 3433;
    seed ^= 0xa7b2c49a;
    return (float(int(seed % 2001) - 1000) / 1000.0f);
}

float Terrain::getSmoothNoise1(int x, int z) {
    float corners = (getNoise(x - 1, z - 1) +
        getNoise(x + 1, z - 1) +
        getNoise(x - 1, z + 1) +
        getNoise(x + 1, z + 1)) / 16.f;
    float sides = (getNoise(x - 1, z)
        + getNoise(x + 1, z)
        + getNoise(x, z - 1)
        + getNoise(x, z + 1)) / 8.f;
    float center = getNoise(x, z) / 4.f;
    return corners + sides + center;
}

float Terrain::getSmoothNoise2(int x, int z) {
    float corners = (getSmoothNoise1(x - 1, z - 1) +
        getSmoothNoise1(x + 1, z - 1) +
        getSmoothNoise1(x - 1, z + 1) +
        getSmoothNoise1(x + 1, z + 1)) / 16.f;
    float sides = (getSmoothNoise1(x - 1, z)
        + getSmoothNoise1(x + 1, z)
        + getSmoothNoise1(x, z - 1)
        + getSmoothNoise1(x, z + 1)) / 8.f;
    float center = getSmoothNoise1(x, z) / 4.f;
    return corners + sides + center;
}

float Terrain::getSmoothNoise3(int x, int z) {
    float corners = (getSmoothNoise2(x - 1, z - 1) +
        getSmoothNoise2(x + 1, z - 1) +
        getSmoothNoise2(x - 1, z + 1) +
        getSmoothNoise2(x + 1, z + 1)) / 16.f;
    float sides = (getSmoothNoise2(x - 1, z)
        + getSmoothNoise2(x + 1, z)
        + getSmoothNoise2(x, z - 1)
        + getSmoothNoise2(x, z + 1)) / 8.f;
    float center = getSmoothNoise2(x, z) / 4.f;
    return corners + sides + center;
}



float Terrain::interpolate(float a, float b, float blend) {
    double theta = blend * glm::pi<double>();
    float smoothness = (float) (1.f - cos(theta)) * 0.5f;
    return a * (1.f - smoothness) + b * smoothness;
}

float Terrain::interpolateNoise(float x, float z) {
    int intX = (int)x;
    int intZ = (int)z;
    float remainderX = x - intX;
    float remainderZ = z - intZ;

    float v1 = getSmoothNoise3(intX, intZ);
    float v2 = getSmoothNoise3(intX + 1, intZ);
    float v3 = getSmoothNoise3(intX, intZ + 1);
    float v4 = getSmoothNoise3(intX + 1, intZ + 1);

    float i1 = interpolate(v1, v2, remainderX);
    float i2 = interpolate(v3, v4, remainderX);
    return interpolate(i1, i2, remainderZ);
}

void Terrain::initHeightMap() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < length; y++) {
            heightMap[x][y] = calculateHeight(x + seed, y + seed);
        }
    }
}
void Terrain::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

}
void Terrain::initBuffers() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GL_UNSIGNED_INT), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Terrain::updateVertices() {
    vertices.clear();
    float blocksize = 1.f;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < length; y++) {
            vertices.push_back(glm::vec3(x*blocksize, heightMap[x][y], y*blocksize));
        }
    }
}

void Terrain::updateIndices() {
    indices.clear();
    for (int x = 0; x < width -1; x++) {
        for (int y = 0; y < length -1; y++) {
            indices.push_back(x*length + y);
            indices.push_back((x + 1)*length + y);
            indices.push_back((x + 1)*length + (y + 1));

            indices.push_back((x + 1)*length + (y + 1));
            indices.push_back(x*length + (y + 1));
            indices.push_back(x*length + y);
        }
    }
}

void Terrain::randomGenerate() {
    seed = rand();
    initHeightMap();
    updateVertices();
    updateIndices();
    initBuffers();
}

void Terrain::draw(GLuint shaderProgram, glm::mat4 C) {
    glm::mat4 model = C;
    glm::mat4 modelview = Window::V * model;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelview"), 1, GL_FALSE, &modelview[0][0]);

    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Terrain::update() {

}

