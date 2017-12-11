#include "LSystemTree.h"

#include "util.h"
#include "Window.h"
#include <time.h> 
#include "include/stb_image.h"
#include <string>

LSystemTree::LSystemTree(int iter, int branchLength, glm::vec3 pos, float dec) {
	// copy parameters into public variables
	iterations = iter;
	length = branchLength;
	position = pos;
	decay = dec;

	rule1.rX = "F[-X][X]F[-X]+FX";
	rule1.rF = "FF";

	rule2.rX = "F";
	rule2.rF = "XX";

	rule3.rX = "";
	rule3.rF = "";

	// Initial direction is "up"
	direction = glm::vec3(0, 1, 0);

	iterate();
}

LSystemTree::~LSystemTree() {

}


void LSystemTree::iterate() {

	// start with sentence as X
	std::deque<char> sentence;
	sentence.push_back('X');

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
					break;

				case '-':
					turnRight();
					break;

				case '[':
					pushPosition();
					break;

				case ']':
					popPosition();
					break;

			}
		}
		printf("This is iteration %d\n", i);

		sentence.clear();
		for (int k = 0; k < nextSentence.size(); k++) {
			sentence.push_back(nextSentence[k]);
		}
		nextSentence.clear();

		for (int k = 0; k < sentence.size(); k++) {
			printf("%c", sentence[k]);
		}
		printf("\n");
	}
}

void LSystemTree::forward() {
	vertices.push_back(lastPosition);
	indices.push_back(vertices.size() - 1);

	glm::vec3 newPosition = lastPosition + ((float)length * glm::normalize(direction));

	vertices.push_back(newPosition);
	indices.push_back(vertices.size() - 1);

	lastPosition = newPosition;
}

void LSystemTree::turnLeft() {
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-25.f), glm::vec3(0, 1, 0));
	direction = glm::vec3(rotationMatrix*glm::vec4(direction, 1));
}

void LSystemTree::turnRight() {
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(25.f), glm::vec3(0, 1, 0));
	direction = glm::vec3(rotationMatrix*glm::vec4(direction, 1));
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

}

void LSystemTree::update() {

}