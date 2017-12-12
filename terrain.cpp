#include "terrain.h"
#include "util.h"
#include "Window.h"
#include <time.h> 
#include "include/stb_image.h"
#include <string>

using namespace util;

Terrain::Terrain(int x, int z, float amp) {
    width = x;
    length = z;
    amplitude = amp;

    std::vector<std::string> texturePaths;
    texturePaths.push_back("../Textures/grass4.jpg");
    texturePaths.push_back("../Textures/grass1.jpg");
    texturePaths.push_back("../Textures/grass2.jpg");
    texturePaths.push_back("../Textures/grass3.jpg");





    setup(texturePaths);
    randomGenerate();

}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Terrain::setup(std::vector<std::string> texturePaths) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &TEXCOORD);

    heightMap = new float*[width];
    for (int i = 0; i < width; i++) {
        heightMap[i] = new float[length];
    }

    for (int i = 0; i < texturePaths.size(); i++) {
        Texture current;
        int width, height;
        int nrChannels;
        unsigned char* image;
        image = stbi_load(texturePaths[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        //Generate a texture
        glGenTextures(1, &current.id);

        //Set all data for GL_TEXTURE_2D to be sent to the generated texture
        glBindTexture(GL_TEXTURE_2D, current.id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        free(image);

        //Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        textures.push_back(current);
    }

}



float Terrain::calculateHeight(int x, int z) {
    float inputX = (float) x/32.f;
    float inputZ = (float) z/32.f;
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

float Terrain::getSmoothNoise(int x, int z) {
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

    float v1 = getSmoothNoise(intX, intZ);
    float v2 = getSmoothNoise(intX + 1, intZ);
    float v3 = getSmoothNoise(intX, intZ + 1);
    float v4 = getSmoothNoise(intX + 1, intZ + 1);

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

void Terrain::updateVertices() {
    vertices.clear();
    float blocksize = 1.f;
    float totalwidth = blocksize * width;
    float totallength = blocksize * length;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < length; y++) {
            vertices.push_back(glm::vec3(x*blocksize - totalwidth/2, heightMap[x][y], y*blocksize - totallength / 2));
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

void Terrain::updateTextureCords() {
    float u = float(width)*0.25f;
    float v = float(length)*0.25f;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < length; y++) {
            float fScaleC = float(y) / float(length);
            float fScaleR = float(x) / float(width);

            textCoord.push_back(glm::vec2(u*fScaleC, v*fScaleR));
        }
    }

}

void Terrain::initBuffers() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GL_UNSIGNED_INT), indices.data() , GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, TEXCOORD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*textCoord.size(), textCoord.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Terrain::randomGenerate() {
    seed = rand();
    initHeightMap();
    updateVertices();
    updateIndices();
    updateTextureCords();
    initBuffers();
}

void Terrain::draw(GLuint shaderProgram, glm::mat4 C) {
    glm::mat4 model = C;
    glm::mat4 modelview = Window::V * model;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelview"), 1, GL_FALSE, &modelview[0][0]);

    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); //switches texture bind location to GL_TEXTURE(0+i)
        glBindTexture(GL_TEXTURE_2D, textures[i].id); //bind texture to active location
        std::string shaderVar = "texture" + std::to_string(i);
        glUniform1i(glGetUniformLocation(shaderProgram, shaderVar.c_str()), i); //sets uniform sampler2D texSampleri's texture bind loc.
    }

    //Draw terrain
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    for (int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Terrain::update() {

}

