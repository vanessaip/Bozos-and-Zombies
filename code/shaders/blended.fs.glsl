#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec4 tex = vec4(texture(sampler0, vec2(texcoord.x, texcoord.y)));
	color = vec4(tex.xy, tex.z / 1.2, 0.85);
}
