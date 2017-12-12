#define _CRT_SECURE_NO_DEPRECATE
#include "Geometry.h"
#include "Window.h"

Geometry::Geometry(const char *filepath)
{
    pos_x = 0;
    pos_y = 0;
    pos_z = 0;
    parse(filepath);
    setup();
    toParentNode = glm::mat4(1.0f);
    center();
    
}

Geometry::Geometry(const char *filepath, bool point)
{
    pos_x = 0;
    pos_y = 0;
    pos_z = 0;
    pointDraw = point;
    parse(filepath);
    setup();
    toParentNode = glm::mat4(1.0f);
    center();

}

Geometry::~Geometry()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &Norms);
}

void Geometry::setup() {
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &Norms);
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.


    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
        3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
        GL_FLOAT, // What type these components are
        GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
        3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
        (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

                     // We've sent the vertex data over to OpenGL, but there's still something missing.
                     // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GL_UNSIGNED_INT), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(Norms);
    glBindBuffer(GL_ARRAY_BUFFER, Norms);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);


    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
}

void Geometry::parse(const char *filepath)
{
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Unable to open file.\n");
    }
    else {
        char header[128];
        while (fscanf(file, "%s", header) != EOF) {
            if (strcmp(header, "v") == 0) {
                glm::vec3 vertex;
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
                vertices.push_back(vertex);
            }
            else if (strcmp(header, "vn") == 0) {
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                normal = glm::normalize(normal);
                normals.push_back(normal);
            }
            else if (strcmp(header, "f") == 0) {
                unsigned int v_indices[3];
                unsigned int n_indices[3];
                fscanf(file, "%d//%d %d//%d %d//%d\n", &v_indices[0], &n_indices[0], &v_indices[1], &n_indices[1], &v_indices[2], &n_indices[2]);
                indices.push_back(v_indices[0] - 1);
                indices.push_back(v_indices[1] - 1);
                indices.push_back(v_indices[2] - 1);
            }
        }
        float x_max = vertices[0].x;
        float x_min = vertices[0].x;
        float y_max = vertices[0].y;
        float y_min = vertices[0].y;
        float z_max = vertices[0].y;
        float z_min = vertices[0].y;
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices[i].x > x_max)
                x_max = vertices[i].x;
            if (vertices[i].y > y_max)
                y_max = vertices[i].y;
            if (vertices[i].z > z_max)
                z_max = vertices[i].z;
            if (vertices[i].x < x_min)
                x_min = vertices[i].x;
            if (vertices[i].y < y_min)
                y_min = vertices[i].y;
            if (vertices[i].z < z_min)
                z_min = vertices[i].z;
        }
        xCenter = (x_max + x_min) / 2.0;
        yCenter = (y_max + y_min) / 2.0;
        zCenter = (z_max + z_min) / 2.0;

        xScale = 2 / (x_max - x_min);
        yScale = 2 / (y_max - y_min);
        zScale = 2 / (z_max - z_min);
    }
    fclose(file);

}

void Geometry::center() {
    this->toParentNode = glm::translate(glm::mat4(1.0f), glm::vec3(0 - xCenter, 0 - yCenter, 0 - zCenter));
}

void Geometry::translate(float x, float y, float z)
{
    toParentNode = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))*toParentNode;
}

void Geometry::rotate(glm::vec3 axis, float deg)
{
    toParentNode = glm::rotate(glm::mat4(1.0f), deg, axis)*toParentNode;
}

void Geometry::scale(float x, float y, float z)
{
    toParentNode = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z))*toParentNode;
}

void Geometry::draw(GLuint shaderProgram, glm::mat4 m)
{
    glm::mat4 model = m * toParentNode;
    glm::mat4 modelview = Window::V * model;
    // Calculate the combination of the model and view (camera inverse) matrices
        
    // Get location in Shader
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    uView = glGetUniformLocation(shaderProgram, "view");
    uModel = glGetUniformLocation(shaderProgram, "model");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
    glUniform4f(glGetUniformLocation(shaderProgram, "inputColor"), color[0], color[1], color[2], color[3]);
    // Now draw the cube. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
    if (pointDraw) {
        glPointSize(4);
        glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
    }        
    else {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }        
    glBindVertexArray(0);  
    
}

void Geometry::update() {
}