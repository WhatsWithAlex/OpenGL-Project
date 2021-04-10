#version 330 core

struct Material 
{
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
}; 
struct Light 
{
    vec3 pos;
    vec3 ambient_intensity;
    vec3 diffuse_intensity;
    vec3 specular_intensity;
}; 
  

out vec4 frag_color;

in vec2 vert_tex_coords;
in vec3 normal;
in vec3 frag_pos;
in vec3 light_pos;

uniform Material material;
uniform Light light;

void main() 
{
    vec3 ambient_light = light.ambient_intensity * vec3(texture(material.diffuse_map, vert_tex_coords));

	vec3 light_dir = normalize(light_pos - frag_pos);
	float diffuse = max(dot(light_dir, normal), 0.0);
	vec3 diffuse_light = diffuse * light.diffuse_intensity * vec3(texture(material.diffuse_map, vert_tex_coords));

	float specular_scale = 0.3;
	vec3 view_dir = -normalize(frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular_light = specular * light.specular_intensity * vec3(texture(material.specular_map, vert_tex_coords));

	frag_color = vec4((ambient_light + diffuse_light + specular_light), 1.0);
}