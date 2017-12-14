#version 330 core
in vec4 mColor;
in vec4 mVertex;

uniform vec4 CameraEye;

out vec4 color;

float getFogFactor(float d)
{
	const float FogMax = 20.0;
    const float FogMin = 10.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main()
{             
	vec4 V = mVertex;
	float d = distance(CameraEye, V);
	float alpha = getFogFactor(d);
	const vec4 FogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	color = mix(mColor, FogColor, alpha);    
}