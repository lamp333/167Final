#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec4 vertex;
in vec2 tCoord;
in vec4 ShadowCoord;

uniform vec4 CameraEye;
uniform float fogFlag;
uniform vec4 inputColor;

//const vec4 FogColor = vec4(0.17f, 0.17f, 0.25f, 1.0f);
const vec4 FogColor = vec4(0.08f, 0.13f, 0.25f, 1.0f);

uniform sampler2DShadow shadow;


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
	float d = length(V-CameraEye);
	float alpha = getFogFactor(d);

	vec4 LightColor = vec4(1,1,1,1);

	float bias = 0.005;
	float visibility = 1.0;
	if ( texture( shadow, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w) )  <  ShadowCoord.z-bias){
		visibility = 0.5;
	}
    color = visibility * inputColor*LightColor;

	if(fogFlag == 1)
	{
		color = mix(color, FogColor, alpha);
	}
}
