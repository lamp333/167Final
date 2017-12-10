#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;
in vec4 vertex;

uniform vec4 CameraEye;
const vec4 FogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

float getFogFactor(float d)
{
	const float FogMax = 80.0;
	const float FogMin = 20.0;

	if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main()
{
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
	vec4 V = vertex;
	float d = distance(CameraEye, V);
	float alpha = getFogFactor(d);
    
	color = vec4(1.0f, 0.41f, 0.7f, sampleExtraOutput);
	color = mix(color, FogColor, alpha);
}
