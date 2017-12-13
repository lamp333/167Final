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
	lastPosition = pos;
	decay = dec;

	rule1.start = "X";
	rule1.rX = "F-[[X]+X]+F[+FX]-X";
	rule1.rF = "[F]F[F]";

	rule2.start = "F";
	rule2.rX = "";
	rule2.rF = "FF+[+F-F-F]-[-F+F+F]";

	rule3.start = "F";
	rule3.rX = "";
	rule3.rF = "F[+F]F[-F]F";

	// Initial direction is "up"
	direction = glm::vec3(0, 1, 0);

    //Scaling branch
    glm::vec4 brown =glm::vec4( 0.5f, 0.26f, 0.1f, 1.f);
    cylinder = new Geometry("../Objects/body.obj");
    cylinder->color = brown;
    cylinder->scale(cylinder->xScale / 8, cylinder->yScale / 8, cylinder->zScale*1.25);

    branch = new Transform(glm::mat4(1.0f));
    branch->rotate(glm::vec3(1, 0, 0), glm::radians(-90.0f));
    branch->rotate(glm::vec3(0, 1, 0), glm::radians(-90.0f));
    branch->translate(0, -0.7f, 0);
    branch->addChild(cylinder);


    //Scaling leafs
    icosahedron = new Geometry("../Objects/leaf.obj");
    icosahedron->scale(2, 2, 2);

	srand(glfwGetTime() * 123);
	int rule = rand() % 3;

	switch (rule) {
	case 0:
		length = 6;
		iterate(rule1);
		break;
	case 1:
		iterate(rule2);
		break;
	case 2:
		iterate(rule3);
		break;

	}
    setup();
}

LSystemTree::~LSystemTree() {
}


void LSystemTree::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, leafVertices.size() * sizeof(glm::vec3), leafVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, leafIndices.size() * sizeof(GL_UNSIGNED_INT), leafIndices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LSystemTree::iterate(Rule rule) {
    tree = new Transform(glm::mat4(1.0f));
    srand(glfwGetTime() * 12356);
	// start with sentence as X
	std::deque<char> sentence;
    for (int i = 0; i < strlen(rule.start); i++) {
        sentence.push_back(rule.start[i]);
    }
	

	for (int i = 0; i < iterations; i++) {

		std::deque<char> nextSentence;
        bool leafStage = false;
        if (i >= iterations - 2) {
            leafStage = true;

        }
		// evaluate axiom using rules
		for (int j = 0; j < sentence.size(); j++) {

			char current = sentence[j];

			switch(current) {
				case 'X':
					// Push the rule for X onto the nextSentence
					for (int k = 0; k < strlen(rule.rX); k++) {
						nextSentence.push_back(rule.rX[k]);
					}
					break;

				case 'F':
					// Push the rule for F onto the nextSentence
					for (int k = 0; k < strlen(rule.rF); k++) {
						nextSentence.push_back(rule.rF[k]);
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
                    if (leafStage) {
                        addLeaf(lastPosition, lastPosition + direction);
                    }
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
	glm::vec3 newPosition = lastPosition + (length * glm::normalize(direction));
    addBranch(lastPosition, newPosition);
    
	lastPosition = newPosition;
}

void LSystemTree::turnLeft() {
    float randomAngleZ = (float)randomBetween(20, 40);
    float randomAngleX = (float)randomBetween(20, 45);
	srand(glfwGetTime() * 12356);
    randomAngleX = (rand() % 2 < 1) ? randomAngleX : -randomAngleX;
	glm::mat4 rotationMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(-randomAngleZ), glm::vec3(0, 0, 1));
    glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(randomAngleX), glm::vec3(0, 1, 0));
	direction = glm::vec3(rotationMatrix1*rotationMatrix2* glm::vec4(direction, 1));


}

void LSystemTree::turnRight() {
    float randomAngleZ = (float)randomBetween(20, 40);
    float randomAngleX = (float)randomBetween(20, 45);
	srand(glfwGetTime() * 12356);
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
    tree->draw(shaderProgram, C);
    // Get location in Shader
    // Now send these values to the shader program
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelview"), 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);


    glm::vec4 green = glm::vec4(0.5f, 0.7f, 0.3f, 1.f);
    glUniform4f(glGetUniformLocation(shaderProgram, "inputColor"), green[0], green[1], green[2], green[3]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, leafIndices.size(), GL_UNSIGNED_INT, 0);
    // Now draw the cube. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
}

void LSystemTree::addBranch(glm::vec3 start, glm::vec3 end) {

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

void LSystemTree::addLeaf(glm::vec3 start, glm::vec3 end) {

    Transform* newLeaf = new Transform(glm::mat4(1.0f));
    newLeaf->addChild(icosahedron);

    //rotate towards end vector
    glm::vec3 v1 = glm::vec3(0, 1, 0);
    glm::vec3 v2 = end - start;
    v1 = glm::normalize(v1);
    v2 = glm::normalize(v2);

    float dotprod = glm::dot(v1, v2);
    float angle = acos(dotprod);

    glm::vec3 axis = glm::cross(v1, v2);
    if (v1 != v2) {
        newLeaf->rotate(axis, angle);
    }

    newLeaf->translate(start.x, start.y, start.z);

    //Add Verticies;
    for (int i = 0; i < icosahedron->vertices.size(); i++) {
        glm::vec3 position = glm::vec3(newLeaf->toParent * icosahedron->toParentNode * glm::vec4(icosahedron->vertices[i], 1.0f));
        leafVertices.push_back(position);
    }
    int offset = leafVertices.size();
    for (int i = 0; i < icosahedron->indices.size(); i++) {
        leafIndices.push_back(offset + icosahedron->indices[i]);
    }

}

void LSystemTree::update() {

}