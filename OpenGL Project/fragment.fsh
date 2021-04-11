#version 330 core

struct Material 
{
    sampler2D diffuse_map;
    sampler2D specular_map;
	sampler2D normal_map;
    float shininess;
}; 
struct DirectedLight 
{
	vec3 dir;
    vec3 ambient_intensity;
    vec3 diffuse_intensity;
    vec3 specular_intensity;
}; 
struct PointLight 
{
    vec3 pos;
    vec3 ambient_intensity;
    vec3 diffuse_intensity;
    vec3 specular_intensity;

	float constant;
	float linear;
	float quadratic;
}; 

out vec4 frag_color;

in vec2 vert_tex_coords;
in vec3 normal;
in vec3 frag_pos;

uniform Material material;
uniform DirectedLight dir_light;
uniform PointLight point_light;
uniform mat4 view;

vec3 calculateDirLight(DirectedLight light, vec3 normal, vec3 frag_pos) 
{
	vec3 light_dir = mat3(view) * light.dir;

	vec3 ambient_light = light.ambient_intensity * vec3(texture(material.diffuse_map, vert_tex_coords));

	float diffuse = max(dot(-light_dir, normal), 0.0);
	vec3 diffuse_light = diffuse * light.diffuse_intensity * vec3(texture(material.diffuse_map, vert_tex_coords));

	vec3 view_dir = -normalize(frag_pos);
	vec3 reflect_dir = reflect(light_dir, normal);
	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular_light = specular * light.specular_intensity * vec3(texture(material.specular_map, vert_tex_coords));

	return ambient_light + diffuse_light + specular_light;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 frag_pos) 
{
	vec3 light_pos = vec3(view * vec4(light.pos, 1.0f));

	float distance = length(light_pos - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient_light = attenuation * light.ambient_intensity * vec3(texture(material.diffuse_map, vert_tex_coords));

	vec3 light_dir = normalize(light_pos - frag_pos);
	float diffuse = max(dot(light_dir, normal), 0.0);
	vec3 diffuse_light = attenuation * diffuse * light.diffuse_intensity * vec3(texture(material.diffuse_map, vert_tex_coords));

	vec3 view_dir = -normalize(frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular_light = attenuation * specular * light.specular_intensity * vec3(texture(material.specular_map, vert_tex_coords));

	return ambient_light + diffuse_light + specular_light;
}

void main() 
{
	vec3 frag_norm = normal * vec3(texture(material.normal_map, vert_tex_coords)); 
	frag_norm = normalize(frag_norm * 2.0 - 1.0); 
	frag_color = vec4(calculateDirLight(dir_light, frag_norm, frag_pos) + calculatePointLight(point_light, frag_norm, frag_pos), 1.0);
}