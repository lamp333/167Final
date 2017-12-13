#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;
in vec4 vertex;
in vec2 tCoord;

uniform vec4 CameraEye;
uniform float fogFlag;

//const vec4 FogColor = vec4(0.17f, 0.17f, 0.25f, 1.0f);
const vec4 FogColor = vec4(0.08f, 0.13f, 0.25f, 1.0f);

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

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

    vec4 texColor;



    if (vertex.y <= -20.f) {
        color = vec4(0.0f, 0.41f, 0.7f, sampleExtraOutput);
    }
    else if (vertex.y <= -15) {
        color = vec4(0.2f, 0.41f, 0.7f, sampleExtraOutput);
    }
    else if (vertex.y <= -5) {
        float range = -5 - (-15);
        float fraction = (vertex.y - (-15))/range;
        texColor = texture(texture0, tCoord)*(1-fraction) + texture(texture1, tCoord)*fraction;
        color = vec4(texColor.xyz, 1.0f);
    }
    else if (vertex.y <= 0) {
        float range = 5;
        float fraction = (vertex.y - (-5))/range;
        texColor = texture(texture1, tCoord)*(1-fraction) + texture(texture2, tCoord)*fraction;
        color = vec4(texColor.xyz, 1.0f);
    }
    else{
        float range = 10 - 0;
        float fraction = (vertex.y - 0)/range;
        texColor = texture(texture2, tCoord)*(1-fraction) + texture(texture3, tCoord)*fraction;
        color = vec4(texColor.xyz, 1.0f);
    }
	

	if(fogFlag == 1)
	{
		color = mix(color, FogColor, alpha);
	}
}
