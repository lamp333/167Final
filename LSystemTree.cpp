#include "LSystemTree.h"

#include "util.h"
#include "Window.h"
#include <time.h> 
#include "include/stb_image.h"
#include <string>

using namespace util;
LSystemTree::LSystemTree(int iter, int branchLength, glm::vec3 pos, float dec) {
	// copy parameters into public variables
	iterations = iter;
	length = branchLength;
	position = pos;
	decay = dec;

	rule1.rX = "F[-X][X]F[-X]+FX";
	rule1.rF = "FF";

	rule2.rX = "";
	rule2.rF = "FF+[+F-F-F]-[-F+F+F]";

	rule3.rX = "";
	rule3.rF = "";

	// Initial direction is "up"
	direction = glm::vec3(0, 1, 0);



    object = new Geometry("../Objects/body.obj");
    object->scale(object->xScale/8, object->yScale/8, object->zScale*1.2);
    branch = new Transform(glm::mat4(1.0f));
    branch->rotate(glm::vec3(1, 0, 0), glm::radians(-90.0f));
    branch->rotate(glm::vec3(0, 1, 0), glm::radians(-90.0f));
    branch->translate(0, -0.7f, 0);
    branch->addChild(object);

    tree = new Transform(glm::mat4(1.0f));

    iterate();
}

LSystemTree::~LSystemTree() {
}



void LSystemTree::iterate() {

	// start with sentence as X
	std::deque<char> sentence;
	sentence.push_back('F');

	for (int i = 0; i < iterations; i++) {

		std::deque<char> nextSentence;

		// evaluate axiom using rules
		for (int j = 0; j < sentence.size(); j++) {

			char current = sentence[j];

			switch(current) {
				case 'X':
					// Push the rule for X onto the nextSentence
					for (int k = 0; k < strlen(rule2.rX); k++) {
						nextSentence.push_back(rule2.rX[k]);
					}
					break;

				case 'F':
					// Push the rule for F onto the nextSentence
					for (int k = 0; k < strlen(rule2.rF); k++) {
						nextSentence.push_back(rule2.rF[k]);
					}
					forward();
					break;

				case '+':
					turnLeft();
                    nextSentence.push_back('+');
					break;

				case '-':
					turnRight();
                    nextSentence.push_back('-');
					break;

				case '[':
					pushPosition();
                    nextSentence.push_back('[');
					break;

				case ']':
					popPosition();
                    nextSentence.push_back(']');
					break;

			}
		}
		
		sentence.clear();
		for (int k = 0; k < nextSentence.size(); k++) {
			sentence.push_back(nextSentence[k]);
		}
		nextSentence.clear();
        length *= decay;
	}
}

void LSystemTree::forward() {
	vertices.push_back(lastPosition);
	indices.push_back(vertices.size() - 1);

	glm::vec3 newPosition = lastPosition + (length * glm::normalize(direction));

	vertices.push_back(newPosition);
	indices.push_back(vertices.size() - 1);
    addBranch(lastPosition, newPosition, 0);
	lastPosition = newPosition;
}

void LSystemTree::turnLeft() {
    float randomAngleZ = (float)randomBetween(20, 40);
    float randomAngleX = (float)randomBetween(20, 45);
    randomAngleX = (rand() % 2 < 1) ? randomAngleX : -randomAngleX;
	glm::mat4 rotationMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(-randomAngleZ), glm::vec3(0, 0, 1));
    glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(randomAngleX), glm::vec3(0, 1, 0));
	direction = glm::vec3(rotationMatrix1*rotationMatrix2* glm::vec4(direction, 1));


}

void LSystemTree::turnRight() {
    float randomAngleZ = (float)randomBetween(20, 40);
    float randomAngleX = (float)randomBetween(20, 45);
    randomAngleX = (rand() % 2 < 1) ? randomAngleX : -randomAngleX;
	glm::mat4 rotationMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(randomAngleZ), glm::vec3(0, 0, 1));
    glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(randomAngleX), glm::vec3(0, 1, 0));
    direction = glm::vec3(rotationMatrix1*rotationMatrix2* glm::vec4(direction, 1));
}

void LSystemTree::pushPosition() {
	stackPositions.push_back(lastPosition);
	stackDirections.push_back(direction);
}

void LSystemTree::popPosition() {

	lastPosition = stackPositions.back();
	direction = stackDirections.back();
	stackPositions.pop_back();
	stackDirections.pop_back();
}

void LSystemTree::draw(GLuint shaderProgram, glm::mat4 C) {
    glm::mat4 model = C;
    glm::mat4 modelview = Window::V * model;
    //ec4(1.0, 0.5, 0.6, 1.0f);
    glUniform4f(glGetUniformLocation(shaderProgram, "inputColor"), 0.5f, 0.26f, 0.1f, 1.f);
    tree->draw(shaderProgram, C);
}

void LSystemTree::addBranch(glm::vec3 start, glm::vec3 end, float thickness) {

    Transform* newBranch = new Transform(glm::mat4(1.0f));
    newBranch->addChild(branch);

    //scale
    float distance = glm::length(end - start);
    newBranch->scale(1, distance, 1);
    newBranch->translate(0, distance / 2, 0);

    //rotate towards end vector
    glm::vec3 v1 = glm::vec3(0, 1, 0);
    glm::vec3 v2 = end - start;
    v1 = glm::normalize(v1);
    v2 = glm::normalize(v2);

    float dotprod = glm::dot(v1, v2);
    float angle = acos(dotprod);

    glm::vec3 axis = glm::cross(v1, v2);
    if (v1 != v2) {
        newBranch->rotate(axis, angle);
    }

    //translate to position
    newBranch->translate(start.x, start.y, start.z);

    tree->addChild(newBranch);

}
void LSystemTree::update() {

}