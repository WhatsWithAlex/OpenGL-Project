#version 330 core

layout(location = 0) in vec3 pos;

out vec3 vert_tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = (projection * view * vec4(pos, 1.0)).xyww;
	vert_tex_coords = pos;
}