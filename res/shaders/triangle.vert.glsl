#version 330 core

layout (location = 0) in vec3 att_pos;

void main()
{
	gl_Position = vec4(att_pos.x, att_pos.y, att_pos.z, 1.0);
}
