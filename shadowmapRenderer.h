#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"
#include "Window.h"
#include "util.h"
#include <vector>

using namespace util;

class shadowmapRenderer {

public:
	GLuint VAO, VBO, EBO, texId;
	shadowmapRenderer(GLuint id);
	~shadowmapRenderer();

	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	void setup();

	void draw(GLuint shaderProgram, glm::mat4 C);

};