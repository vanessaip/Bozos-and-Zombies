#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float screen_darken_factor;
uniform bool is_poisoned;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec2 distort(vec2 uv) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE WATER WAVE DISTORTION HERE (you may want to try sin/cos)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
if (!is_poisoned){
	return uv;
}
	vec2 distortion = vec2((sin(time/10)+39)/40, (cos(time/10)+39)/40);
	return distortion*uv;
}

vec4 color_shift(vec4 in_color) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE COLOR SHIFTING HERE (you may want to make it blue-ish)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
if (!is_poisoned){
	return in_color;
}
	vec4 shift = vec4(0.8,0.8,1.2,0);
	return shift*in_color;
}

vec4 fade_color(vec4 in_color) 
{
	if (screen_darken_factor > 0)
		in_color -= screen_darken_factor * vec4(0.8, 0.8, 0.8, 0);
	return in_color;
}

void main()
{
		vec2 coord = distort(texcoord);
		vec4 in_color = texture(screen_texture, coord);
		color = color_shift(in_color);
		color = fade_color(color);
	
}