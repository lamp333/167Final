#include "Node.h"
#include <deque>

#include "Geometry.h"
#include "Transform.h"
class LSystemTree {

public:
    struct Rule {
        char * start;
        char * rX;
        char * rF;
    };

	LSystemTree(int iter, int branchLength, glm::vec3 pos, float dec);
	~LSystemTree();

	void iterate(Rule rule);			        // action on 'X'
	void forward();   // action on 'F'
	void turnLeft();		        // action on '+'
	void turnRight();	        	// action on '-'
	void pushPosition();	        // action on '['
	void popPosition();		        // action on ']

	void draw(GLuint shaderProgram, glm::mat4 C);
	void shadowDraw();
	void update();
    void setup();

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

	GLuint branchVAO, branchVBO, branchEBO, branchNBO;
	std::vector<glm::vec3> branchVertices;
	std::vector<glm::vec3> branchNormals;
	std::vector<GLuint> branchIndices;

    GLuint leafVAO, leafVBO, leafEBO, leafNBO;
	std::vector<glm::vec3> leafVertices;
	std::vector<glm::vec3> leafNormals;
	std::vector<GLuint> leafIndices;

	// for use with push and popPosition
	std::deque<glm::vec3> stackPositions;
	std::deque<glm::vec3> stackDirections;
	
	// floats for randomized colors
	float greenR, greenG, greenB;

	// variables for L-System Generation
	Rule rule1;
	Rule rule2;
	Rule rule3;

};