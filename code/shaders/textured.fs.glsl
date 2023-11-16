#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int label;
uniform float time;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	if(label == 1){
	  if(time < 5.f) {
	    color = color * cos(0.5* time);  
	  } else {
	    color.a = 0;
	  }
	}
}
