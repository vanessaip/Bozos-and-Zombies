#version 330

// From vertex shader
in vec2 texcoord;
in vec4 worldPos;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float fading_factor;
uniform vec3 lights[2]; // xy - position, z - intensity

// Output color
layout(location = 0) out  vec4 color;

vec4 fade_color(vec4 in_color) 
{
	if (fading_factor > 0)
		in_color = in_color * fading_factor;
	return in_color;
}

void main()
{
	color = vec4(fcolor, 1) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	color = fade_color(color);
	
	/*
	if (worldPos.x < 400.f) 
	{
		vec3 bColor = vec3(0.0);
		color.xyz = bColor.xyz;
	}
	*/

	bool isLit = false;
	vec3 baseColor = vec3(0.01, 0.01, 0.0);
	for (int i = 0; i < 2; i++) 
	{
		vec3 light = lights[i];
		float lightIntensity = 300.0 / pow(distance(worldPos.xy, light.xy), light.z);

		baseColor.xyz += lightIntensity * color.xyz;
		//color.x = min(1, color.x);
		//color.y = min(1, color.y);
		//color.z = min(1, color.z);
	}
	//color.x = min(color.x, baseColor.x);
	//color.y = min(color.y, baseColor.y);
	//color.z = min(color.z, baseColor.z);
	color.xyz = vec3(min(baseColor.x, 1), min(baseColor.y, 1), min(baseColor.z, 1));
	
	/*
	if(!isLit) 
	{
		vec3 bColor = vec3(0.0);
		color.xyz = bColor.xyz;
	}
	*/
}
