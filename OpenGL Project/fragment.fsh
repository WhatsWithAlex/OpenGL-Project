#version 330 core

out vec4 frag_color;

in vec2 vert_tex_coords;
in vec3 normal;
in vec3 frag_pos;
in vec3 light_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 light_color;

void main() 
{
	float ambient_scale = 0.03;
    vec3 ambient_light = ambient_scale * light_color;

	vec3 light_dir = normalize(light_pos - frag_pos);
	float diffuse = max(dot(light_dir, normal), 0.0);
	vec3 diffuse_light = diffuse * light_color;

	frag_color = vec4((ambient_light + diffuse_light), 1.0) * texture(texture1, vert_tex_coords);
}