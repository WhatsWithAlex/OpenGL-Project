#version 330 core

out vec4 frag_color;

in vec3 vert_tex_coords;

uniform samplerCube skybox;

void main()
{
	frag_color = texture(skybox, vert_tex_coords);
}