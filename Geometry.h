#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_
#include "Node.h"
class Geometry : public Node {
public:
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    glm::mat4 toParentNode;

    GLuint VBO, VAO, EBO, Norms;
    GLuint uProjection, uModelview, uModel, uView;

    float pos_x;
    float pos_y;
    float pos_z;

    float xCenter = 0.0;
    float yCenter = 0.0;
    float zCenter = 0.0;

    float xScale;
    float yScale;
    float zScale;
    bool pointDraw;

    Geometry(const char* filepath);
    Geometry(const char *filepath, bool point);
    ~Geometry();

    void parse(const char* filepath);
    void setup();
    void center();
    void draw(GLuint shaderProgram, glm::mat4 m);
    void update();
    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(glm::vec3 axis, float deg);
};

#endif