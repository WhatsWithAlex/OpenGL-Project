#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm_vec;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 tex_coords;

out vec2 vert_tex_coords;
out vec3 normal;
out vec3 frag_pos;
out vec4 frag_light_pos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space;

void main() 
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	vert_tex_coords = tex_coords;
	frag_pos = vec3(view * model * vec4(pos, 1.0));
	frag_light_pos = light_space * model * vec4(pos, 1.0);

	vec3 T = normalize(vec3(view * model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(view * model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(view * model * vec4(norm_vec, 0.0)));
	TBN = mat3(T, B, N);
}