#version 330 core

layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec3 att_color;

out vec3 our_color;

void main()
{
	gl_Position = vec4(att_pos, 1.0);
	our_color = att_color;
}
