#version 330
#define NUM_OF_LIGHTS 8

// From vertex shader
in vec2 texcoord;
in vec4 worldPos;


// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform bool hasLights;
uniform vec3 lights[NUM_OF_LIGHTS]; // xy - position, z - intensity

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1) * texture(sampler0, vec2(texcoord.x, texcoord.y));

	/*
	if (worldPos.x < 400.f) 
	{
		vec3 bColor = vec3(0.0);
		color.xyz = bColor.xyz;
	}
	*/

	if (hasLights) 
	{
		bool isLit = false;
		vec3 baseColor = vec3(0.01, 0.01, 0.0);
		for (int i = 0; i < NUM_OF_LIGHTS; i++) 
		{
			vec3 light = lights[i];
			float dist = distance(worldPos.xy, light.xy);

			float lightIntensity = 300.0 / pow(dist, light.z);

			baseColor.xyz += lightIntensity * color.xyz;
		
			if (dist < light.z * 100.0) 
			{
				isLit = true;
			}
		}
		color.xyz = vec3(min(baseColor.x, 1), min(baseColor.y, 1), min(baseColor.z, 1));

		
		if (isLit) 
		{
			color.z /= 1.3;
			//color.xyz = vec3(min(baseColor.x, 1), min(baseColor.y, 1), min(baseColor.z, 1));
		}
		else 
		{
			//vec3 bColor = vec3(0.0);
			//color.xyz = bColor.xyz;
		}
		
	}
}
