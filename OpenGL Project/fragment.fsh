#version 330 core

#define TEX_NUM 3
#define LGT_NUM 5

struct Material 
{
    sampler2D diffuse_map[TEX_NUM];
    sampler2D specular_map[TEX_NUM];
	sampler2D normal_map[TEX_NUM];
	sampler2D emission_map[TEX_NUM];
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
in vec4 frag_light_pos;
in mat3 TBN;

uniform sampler2D shadow_map;
uniform Material material;
uniform DirectedLight dir_light;
uniform PointLight point_light[LGT_NUM];
uniform mat4 model;
uniform mat4 view;

float calculateShadow(vec4 frag_light_pos, vec3 normal, vec3 light_dir) 
{
	vec3 projection_coords = frag_light_pos.xyz / frag_light_pos.w;
	projection_coords = projection_coords * 0.5 + 0.5;
	float closest = texture(shadow_map, projection_coords.xy).r;
	float current = projection_coords.z;
		
	float offset = max(0.1 * (1.0 - dot(normal, light_dir)), 0.01);
	return (current - offset > closest) && (projection_coords.z <= 1.0) ? 1.0 : 0.0;
}

vec3 calculateDirLight(DirectedLight light, vec3 normal, vec3 frag_pos) 
{
	vec3 light_dir = mat3(view) * light.dir;

	vec3 ambient_light = light.ambient_intensity * vec3(texture(material.diffuse_map[0], vert_tex_coords));

	float emission = max(dot(light_dir, normal), 0.0);
	vec3 emission_light = emission * vec3(texture(material.emission_map[0], vert_tex_coords));

	float diffuse = max(dot(-light_dir, normal), 0.0);
	vec3 diffuse_light = diffuse * light.diffuse_intensity * vec3(texture(material.diffuse_map[0], vert_tex_coords));

	vec3 view_dir = -normalize(frag_pos);
	vec3 half_dir = normalize(-light_dir + view_dir);
	float specular = pow(max(dot(half_dir, normal), 0.0), material.shininess);
	vec3 specular_light = specular * light.specular_intensity * vec3(texture(material.specular_map[0], vert_tex_coords));

	float shadow = calculateShadow(frag_light_pos, normal, -light_dir);
	return ambient_light + emission_light + (1.0 - shadow) * (diffuse_light + specular_light);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 frag_pos) 
{
	vec3 light_pos = vec3(view * vec4(light.pos, 1.0));

	float distance = length(light_pos - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient_light = attenuation * light.ambient_intensity * vec3(texture(material.diffuse_map[0], vert_tex_coords));

	vec3 emission_light = vec3(texture(material.emission_map[0], vert_tex_coords));

	vec3 light_dir = normalize(light_pos - frag_pos);
	float diffuse = max(dot(light_dir, normal), 0.0);
	vec3 diffuse_light = attenuation * diffuse * light.diffuse_intensity * vec3(texture(material.diffuse_map[0], vert_tex_coords));

	vec3 view_dir = -normalize(frag_pos);
	vec3 half_dir = normalize(light_dir + view_dir);
	float specular = pow(max(dot(half_dir, normal), 0.0), material.shininess);
	vec3 specular_light = attenuation * specular * light.specular_intensity * vec3(texture(material.specular_map[0], vert_tex_coords));

	float shadow = calculateShadow(frag_light_pos, normal, light_dir);
	return ambient_light + emission_light + (1.0 - shadow) * diffuse_light + specular_light;
}

void main() 
{
	vec3 frag_norm = vec3(texture(material.normal_map[0], vert_tex_coords));
	frag_norm = frag_norm * 2.0 - 1.0;
	frag_norm = normalize(TBN * frag_norm);

	vec3 result = calculateDirLight(dir_light, frag_norm, frag_pos);
	for (int i = 0; i < 0; ++i)
		result += calculatePointLight(point_light[i], frag_norm, frag_pos);
	frag_color = vec4(result, 1.0);
}