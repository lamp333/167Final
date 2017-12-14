
#version 330 core
layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec4 Color;

uniform mat4 ModelViewProjectionMatrix;

out vec4 mColor;
out vec4 mVertex;

void main()
{
    mColor = Color;
	mVertex = Vertex;
	gl_Position = ModelViewProjectionMatrix * Vertex;
}  