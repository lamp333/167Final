#include "Transform.h";
Transform::Transform(glm::mat4 matrix)
{
    toParent = matrix;
}
Transform::~Transform()
{
    for (int i = 0; i < children.size(); i++)
    {
        delete children[i];
    }
}
void Transform::draw(GLuint shaderProgram, glm::mat4 C)
{
    for (int i = 0; i < children.size(); i++)
    {
        children[i]->draw(shaderProgram, C*toParent);
    }
}

void Transform::addChild(Node* child)
{
    children.push_back(child);
}

void Transform::removeChild(Node * child)
{
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i] == child)
        {
            children.erase(children.begin() + i);
            return;
        }
    }
}

void Transform::update()
{
    for (int i = 0; i < children.size(); i++)
    {
        children[i]->update();
    }
}

void Transform::translate(float x, float y, float z)
{
    toParent = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))*toParent;
}

void Transform::rotate(glm::vec3 axis, float deg)
{
    toParent = glm::rotate(glm::mat4(1.0f), deg, axis)*toParent;
}

void Transform::scale(float x, float y, float z)
{
    toParent = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z))*toParent;
}