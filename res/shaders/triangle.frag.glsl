#version 330 core

in vec3 our_color;
in vec2 tex_coord;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 frag_color;

void main()
{
	frag_color = (mix(texture(texture1, tex_coord),
					 texture(texture2, tex_coord), 0.2));
	// frag_color = texture(texture1, tex_coord) * vec4(our_color, 1.0);
	// frag_color = vec4(our_color, 1.0);
}
