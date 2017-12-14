#version 330 core
// This is a sample fragment shader.
in vec4 vertex;
in vec2 tCoord;
in vec4 ShadowCoord;

uniform vec4 CameraEye;
uniform float fogFlag;
uniform bool drawShadow;

//const vec4 FogColor = vec4(0.17f, 0.17f, 0.25f, 1.0f);
const vec4 FogColor = vec4(0.08f, 0.13f, 0.25f, 1.0f);

uniform sampler2DShadow shadow;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

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
        color = vec4(0.0f, 0.41f, 0.7f, 1.0f);
    }
    else if (vertex.y <= -15) {
        color = vec4(0.2f, 0.41f, 0.7f, 1.0f);
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


	float bias = 0.005;
	float visibility = 1.0;
	for (int i=0;drawShadow && i<16;i++){
		visibility-=0.004*i*(1 - texture( shadow, vec3(ShadowCoord.xy + poissonDisk[i]/700.0, (ShadowCoord.z-bias)/ShadowCoord.w)));
	}
    vec4 newColor = visibility * color;

	color = newColor;

	if(fogFlag == 1)
	{
		color = mix(color, FogColor, alpha);
	}
}
