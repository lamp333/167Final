#include "Node.h"


class Terrain {

public:

    struct Texture {
        GLuint id;
        std::string shaderVar;
    };

    float** heightMap;

    int width;
    int length;
    int seed;

    float amplitude;
    GLuint VAO, VBO, EBO, TEXCOORD;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textCoord;
    std::vector<GLuint> indices;

    std::vector<Texture> textures;

    Terrain(int x, int y, float amp);
    ~Terrain();
    void draw(GLuint shaderProgram, glm::mat4 C);
	void shadowDraw();
    void update();
    void updateVertices();
    void updateIndices();
    void updateTextureCords();

    float calculateHeight(int x, int z);
    float getNoise(int x, int z);
    float getSmoothNoise(int x, int z);
    float interpolate(float a, float b, float blend);
    float interpolateNoise(float x, float z);

    void randomGenerate();

    void setup(std::vector<std::string> textures);
    void initHeightMap();
    void initBuffers();
};