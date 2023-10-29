#version 330

in vec3 vcolor;
in vec2 vpos; // Distance from local origin

uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int light_up;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(fcolor * vcolor, 1.0);

    float radius = distance(vec2(0.0), vpos);
    if (light_up == 1 && radius < 0.15)
    {
        color.xyz += (0.15 - radius) * 1.2 * vec3(1.0, 1.0, 0.0);
    }
}
