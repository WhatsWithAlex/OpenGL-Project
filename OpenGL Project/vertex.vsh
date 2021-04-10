#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec3 norm_vec;

out vec2 vert_tex_coords;
out vec3 normal;
out vec3 frag_pos;
out vec3 light_pos;

uniform vec3 light_position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main() 
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	frag_pos = vec3(view * model * vec4(pos, 1.0));
	vert_tex_coords = tex_coords;
	normal = normal_matrix * norm_vec;
	light_pos = vec3(view * vec4(light_position, 1.0));
}