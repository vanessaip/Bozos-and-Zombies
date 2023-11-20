#version 330

in vec3 in_position;
in vec3 in_color;

out vec3 vcolor;
flat out int spikeFlag; // Flag to indicate spike or wheel
out vec2 vpos;

uniform mat3 transform;
uniform mat3 projection;

void main()
{
    vpos = in_position.xy; 
    vcolor = in_color * (1.0 - in_position.z); // Gradient effect based on z-coordinate

    // Determine if the vertex belongs to a spike
    spikeFlag = in_position.z > 0.01 ? 1 : 0; // Spike if z > 0.01, else wheel

    vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
    gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
