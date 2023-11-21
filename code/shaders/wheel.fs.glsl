#version 330

in vec3 vcolor;
flat in int spikeFlag; // Receive the flag from vertex shader
in vec2 vpos;

uniform sampler2D sampler0;
uniform vec3 wheelColor; // Color for the wheel
uniform vec3 spikeColor; // Color for the spikes
uniform int light_up;

layout(location = 0) out vec4 color;

void main()
{
    vec3 baseColor = spikeFlag == 1 ? spikeColor : wheelColor;
    color = vec4(baseColor * vcolor, 1.0);

    float radius = distance(vec2(0.0), vpos);
    if (light_up == 1 && radius < 0.15)
    {
        color.xyz += (0.15 - radius) * 1.2 * vec3(1.0, 1.0, 0.0); // Lighting effect
    }
}
