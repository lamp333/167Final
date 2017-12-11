#include "Node.h"
#include <deque>

class LSystemTree {

public:
	LSystemTree(int iter, int branchLength, glm::vec3 pos, float dec);
	~LSystemTree();

	void iterate();			// action on 'X'
	void forward();			// action on 'F'
	void turnLeft();		// action on '+'
	void turnRight();		// action on '-'
	void pushPosition();	// action on '['
	void popPosition();		// action on ']'

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update();

	// parameters from constructor
	int iterations;
	int length;
	glm::vec3 position;
	float decay;

	glm::vec3 lastPosition;
	glm::vec3 direction;

	// places to make our lines/cylinders
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	// for use with push and popPosition
	std::deque<glm::vec3> stackPositions;
	std::deque<glm::vec3> stackDirections;

	// variables for L-System Generation
	struct Rule {
		char * rX;
		char * rF;
	};

	Rule rule1;
	Rule rule2;
	Rule rule3;

};