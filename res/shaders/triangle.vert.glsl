#version 330 core

layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec2 att_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tex_coord;

void main()
{
	gl_Position = projection * view * model * vec4(att_pos, 1.0);
	tex_coord = att_tex_coord;
}
