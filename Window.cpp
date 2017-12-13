#include "window.h"
#include "skybox.h"
#include "ParticleSystem.h"
#include "terrain.h"
#include "LSystemTree.h"

// Sound system for background music
#include "include/irrKlang.h"
#include "include/ik_ISound.h"
#pragma comment(lib, "irrKlang.lib")

using namespace irrklang;

const char* window_title = "GLFW Starter Project";
GLint shaderProgram;
GLint skyboxShader;
GLint particleShader;
GLint terrainShader;

int fogFlag = 1;

Cube * cube;

// Background music engine
ISoundEngine* engine;
ISoundEngine* playerSFX;
bool shouldPlay = true;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

// Default camera parameters
glm::vec3 Window::cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 Window::cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 Window::cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

GLuint Window::FBO, Window::depthTexture;
int Window::width;
int Window::height;


bool selectCamera = false;
double lastTime = glfwGetTime();

double delta;
float horizontalAngle = glm::radians(180.0f);
float verticalAngle = 0.0f;
float mouseSpeed = 0.05f;
double click_xpos, click_ypos;
bool leftClick = false;
bool rightClick = false;
bool generate = true;

double mouse_x;
double mouse_y;

glm::mat4 Window::P;
glm::mat4 Window::V;

Skybox * skybox;
ParticleSystem * particleSys1;
ParticleSystem * particleSys2;
ParticleSystem * particleSys3;

Terrain* terrain;

std::vector<LSystemTree*> trees;

//LSystemTree* tree;

void Window::initialize_objects()
{
    skybox = new Skybox();
    std::vector<const GLchar*> faces =
    {
        "../Skybox/right.ppm",
        "../Skybox/left.ppm",
        "../Skybox/top.ppm",
        "../Skybox/bottom.ppm",
        "../Skybox/front.ppm",
        "../Skybox/back.ppm"
    };
    skybox->loadCubemap(faces);
	cube = new Cube();

	playerSFX = createIrrKlangDevice();
	playerSFX->play2D("../Music/wingbeatslow.wav", true);
	playerSFX->setSoundVolume(0.0);

    particleSys1 = new ParticleSystem("../Textures/firefly1.png", 100);
    particleSys2 = new ParticleSystem("../Textures/firefly2.png", 100);
    particleSys3 = new ParticleSystem("../Textures/firefly3.png", 100);

    terrain = new Terrain(500, 500, 30);

	

	for (int i = 0; i < 128; i++)
	{
		int xRand = (rand() % 200)+150;
		int zRand = (rand() % 200)+150;
		int y = terrain->heightMap[xRand][zRand];
		srand(glfwGetTime() * 123 + (456 * i));
		LSystemTree * tree = new LSystemTree(3, 3, glm::vec3(xRand-250, y-1, zRand-250), 0.6);
		trees.push_back(tree);
	}

	//tree = new LSystemTree(3, 3, glm::vec3(0,0,0), 0.6);
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    skyboxShader = LoadShaders("../Shaders/skyboxShader.vert", "../Shaders/skyboxShader.frag");
    particleShader = LoadShaders("../Shaders/particle.vert", "../Shaders/particle.frag");
    terrainShader = LoadShaders("../Shaders/terrain.vert", "../Shaders/terrain.frag");

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	engine->drop();
	playerSFX->drop();
	delete(skybox);
	glDeleteProgram(shaderProgram);
    glDeleteProgram(skyboxShader);
    glDeleteProgram(particleShader);
    glDeleteProgram(terrainShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	// Background music
	engine = createIrrKlangDevice();
	engine->play2D("../Music/mahouforest.mp3", true);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
}

void Window::display_callback(GLFWwindow* window)
{
    double currentTime = glfwGetTime();
    delta = currentTime - lastTime;
    lastTime = currentTime;

    renderScene();

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::renderShadow() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, 1024, 1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}
void Window::renderScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    Window::V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Tree
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "fogFlag"), fogFlag);
    glUniform4f(glGetUniformLocation(shaderProgram, "CameraEye"), cam_pos.x, cam_pos.y, cam_pos.z, 1.0f);
    //tree->draw(shaderProgram, glm::mat4(1.0f));
	for (int i = 0; i < 128; i++)
	{
		trees[i]->draw(shaderProgram, glm::mat4(1.0f));
	}


    //terrain
    glUseProgram(terrainShader);
    glUniform1f(glGetUniformLocation(terrainShader, "fogFlag"), fogFlag);
    glUniform4f(glGetUniformLocation(terrainShader, "CameraEye"), cam_pos.x, cam_pos.y, cam_pos.z, 1.0f);
    terrain->draw(terrainShader, glm::mat4(1.0f));

    // skybox
    glUseProgram(skyboxShader);
    glUniform4f(glGetUniformLocation(skyboxShader, "CameraEye"), cam_pos.x, cam_pos.y, cam_pos.z, 1.0f);
    skybox->draw(skyboxShader);

    //Particles
    glUseProgram(particleShader);
    if (generate) {
        particleSys1->generate(delta, 500, 30, 500);
        particleSys2->generate(delta, 500, 30, 500);
        particleSys3->generate(delta, 500, 30, 500);
    }
    particleSys1->render(particleShader);
    particleSys2->render(particleShader);
    particleSys3->render(particleShader);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action != GLFW_RELEASE)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
        else if (key == GLFW_KEY_C)
        {
            selectCamera = !selectCamera;
            glfwSetCursorPos(window, Window::width / 2, Window::height / 2);
            if (selectCamera)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            else
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else if (key == GLFW_KEY_W)
        {
            glm::vec3 dir = glm::normalize(cam_look_at - cam_pos);
			dir *= 0.45;
            cam_pos += dir;
            cam_look_at += dir;
			if (shouldPlay) {
				playerSFX->setSoundVolume(1.0);
			}
        }
        else if (key == GLFW_KEY_A)
        {
            glm::vec3 lookat = (cam_look_at - cam_pos);
            glm::vec3 dir = glm::normalize(glm::cross(cam_up, lookat));
			dir *= 0.45;
            cam_pos += dir;
            cam_look_at += dir;
			if (shouldPlay) {
				playerSFX->setSoundVolume(1.0);
			}
		}
        else if (key == GLFW_KEY_S)
        {
            glm::vec3 dir = glm::normalize(cam_look_at - cam_pos);
			dir *= 0.45;
            cam_pos -= dir;
            cam_look_at -= dir;
			if (shouldPlay) {
				playerSFX->setSoundVolume(1.0);
			}
		}
        else if (key == GLFW_KEY_D)
        {
            glm::vec3 lookat = (cam_look_at - cam_pos);
            glm::vec3 dir = glm::normalize(glm::cross(lookat, cam_up));
			dir *= 0.45;
            cam_pos += dir;
            cam_look_at += dir;
			if (shouldPlay) {
				playerSFX->setSoundVolume(1.0);
			}
		}
        else if (key == GLFW_KEY_G)
        {
            generate = !generate;
        }
        else if (key == GLFW_KEY_R)
        {
            terrain->randomGenerate();
        }
        else if (key == GLFW_KEY_SPACE)
        {
            if (mods != GLFW_MOD_SHIFT) {
                cam_pos += glm::vec3(0, 1, 0);
                cam_look_at += glm::vec3(0, 1, 0);
            }
            else if (mods == GLFW_MOD_SHIFT) {
                cam_pos -= glm::vec3(0, 1, 0);
                cam_look_at -= glm::vec3(0, 1, 0);
            }
        }

		else if (key == GLFW_KEY_F)
		{
			if (fogFlag == 1)
			{
				fogFlag = 0;
			}

			else
			{
				fogFlag = 1;
			}
            glUniform1f(glGetUniformLocation(terrainShader, "fogFlag"), fogFlag);
            glUniform1f(glGetUniformLocation(shaderProgram, "fogFlag"), fogFlag);
		}

		else if (key == GLFW_KEY_M)
		{
			if (shouldPlay) {
				shouldPlay = false;
				engine->setSoundVolume(0.0);
				playerSFX->setSoundVolume(0.0);
			}
			else {
				shouldPlay = true;
				engine->setSoundVolume(1.0);
				playerSFX->setSoundVolume(0.0);
			}
		}
	}
	else {
		playerSFX->setSoundVolume(0.0);
	}
}



void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (xpos >= 0 && xpos < Window::width && ypos >= 0 && ypos < Window::height) {
        mouse_x = xpos;
        mouse_y = Window::height - ypos;
        if (selectCamera) {
            glfwSetCursorPos(window, Window::width / 2, Window::height / 2);
            horizontalAngle += mouseSpeed * delta * float(Window::width / 2 - xpos);
            verticalAngle += mouseSpeed * delta * float(Window::height / 2 - ypos);
            glm::vec3 direction(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
            );
            glm::vec3 right = glm::vec3(
                sin(horizontalAngle - glm::radians(180.0f) / 2.0f),
                0,
                cos(horizontalAngle - glm::radians(180.0f) / 2.0f)
            );
            cam_up = glm::cross(right, direction);
            cam_look_at = cam_pos + direction;
        }

    }

}



void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        glfwGetCursorPos(window, &click_xpos, &click_ypos);
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            leftClick = true;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            rightClick = true;
        }
    }
    if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            leftClick = false;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            rightClick = false;
        }

    }

}