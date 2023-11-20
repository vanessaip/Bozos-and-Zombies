#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float fading_factor;

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
}
