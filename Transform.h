#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_
#include "Node.h"

class Transform : public Node {

public:
    glm::mat4 toParent;
    std::vector<Node*> children;

    Transform(glm::mat4 m);
    ~Transform();

    void addChild(Node* child);
    void removeChild(Node* child);
    void draw(GLuint shaderProgram, glm::mat4 C);
    void update();
    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(glm::vec3 axis, float deg);

};
#endif