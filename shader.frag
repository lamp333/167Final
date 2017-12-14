#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec4 vertex;
in vec2 tCoord;
in vec4 ShadowCoord;
in vec3 norm;

uniform vec4 CameraEye;
uniform vec3 light_position;
uniform float fogFlag;
uniform vec4 inputColor;
uniform bool drawShadow;
uniform bool drawRim;
uniform vec3 RimColor;

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
	vec4 pos = vertex;
	float d = length(pos-CameraEye);
	float alpha = getFogFactor(d);

	vec3 L = normalize( light_position - pos.xyz);
	vec3 V = normalize( CameraEye.xyz - pos.xyz);

	float rim = 1 - max(dot(V, norm), 0.0) +0.22;
	rim = smoothstep(0.1, 0.6, rim);
	vec3 finalRim = RimColor * vec3(rim, rim, rim);

	vec4 totalColor = inputColor + vec4(finalRim, 0);

	float bias = 0.005;
	float visibility = 1.0;
	if ( drawShadow && texture( shadow, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w) )  <  ShadowCoord.z-bias){
		visibility = 0.5;
	}
	color =  inputColor*visibility;
	if( drawRim){
		color =  color + vec4(finalRim, 0);
	}

	if(fogFlag == 1)
	{
		color = mix(color, FogColor, alpha);
	}
}
