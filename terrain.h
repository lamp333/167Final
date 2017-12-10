#include "Node.h"


class Terrain {

public:
    float** heightMap;

    float randomMagnitude;
    int mapComplexity;
    int level;

    int width;
    int length;
    int seed;

    float amplitude;
    GLuint VAO, VBO, EBO;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;

    Terrain(int x, int y, float amp);
    ~Terrain();
    void draw(GLuint shaderProgram, glm::mat4 C);
    void update();
    void updateVertices();
    void updateIndices();

    float calculateHeight(int x, int z);
    float getNoise(int x, int z);
    float getSmoothNoise1(int x, int z);
    float getSmoothNoise2(int x, int z);
    float getSmoothNoise3(int x, int z);
    float interpolate(float a, float b, float blend);
    float interpolateNoise(float x, float z);

    void randomGenerate();

    void setup();
    void initHeightMap();
    void initBuffers();
};