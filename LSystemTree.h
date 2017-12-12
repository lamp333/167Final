#include "Node.h"
#include <deque>

#include "Geometry.h"
#include "Transform.h"
class LSystemTree {

public:
	LSystemTree(int iter, int branchLength, glm::vec3 pos, float dec);
	~LSystemTree();

	void iterate();			        // action on 'X'
	void forward();   // action on 'F'
	void turnLeft();		        // action on '+'
	void turnRight();	        	// action on '-'
	void pushPosition();	        // action on '['
	void popPosition();		        // action on ']'

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update();

    void addBranch(glm::vec3 from, glm::vec3 to);
    void addLeaf(glm::vec3 from, glm::vec3 to);

	// parameters from constructor
	int iterations;
	float length;
	glm::vec3 position;
	float decay;
	glm::vec3 lastPosition;
	glm::vec3 direction;


    Geometry* cylinder;
    Geometry* icosahedron;
    Transform* branch;
    Transform* leaf;
    Transform* tree;
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