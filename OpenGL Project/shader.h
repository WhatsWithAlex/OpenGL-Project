#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader 
{
	GLuint shader_id;
	void checkCompileStatus(GLuint shader, GLint type);
public:
	Shader(const std::string &vertex_shader_path, const std::string &fragment_shader_path);
	GLuint getID();
	void use();
	void setUniform(const std::string &name, GLint value) const;
	void setUniform(const std::string &name, GLfloat value) const;
	void setUniform(const std::string &name, glm::mat3 value) const;
	void setUniform(const std::string &name, glm::mat4 value) const;
	void setUniform(const std::string &name, glm::vec3 value) const;
};

void Shader::checkCompileStatus(GLuint shader, GLint type) 
{
	GLint success;
	char info_log[1024];

	glGetShaderiv(shader, type, &success);
	if (!success) 
	{
		glGetShaderInfoLog(shader, 1024, NULL, info_log);
		std::cout << "Error while compiling shader\n" << info_log << std::endl;
	}
}
Shader::Shader(const std::string &vertex_shader_path, const std::string &fragment_shader_path) 
{
	const char *vertex_shader_src;
	const char *fragment_shader_src;
	std::string vertex_shader_src_s;
	std::string fragment_shader_src_s;
	std::ifstream vertex_shader_file;
	std::ifstream fragment_shader_file;

	vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try 
	{
		vertex_shader_file.open(vertex_shader_path);
		fragment_shader_file.open(fragment_shader_path);

		std::stringstream vertex_shader_stream;
		std::stringstream fragment_shader_stream;
		vertex_shader_stream << vertex_shader_file.rdbuf();
		fragment_shader_stream << fragment_shader_file.rdbuf();

		vertex_shader_file.close();
		fragment_shader_file.close();

		vertex_shader_src_s = vertex_shader_stream.str();
		fragment_shader_src_s = fragment_shader_stream.str();
	} 
	catch (std::ifstream::failure &a) 
	{
		std::cout << "Error opening file!\n";
	};
	
	vertex_shader_src = vertex_shader_src_s.c_str();
	fragment_shader_src = fragment_shader_src_s.c_str();

	GLuint vertex_shader, fragment_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
	glCompileShader(vertex_shader);
	checkCompileStatus(vertex_shader, GL_COMPILE_STATUS);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
	glCompileShader(fragment_shader);
	checkCompileStatus(fragment_shader, GL_COMPILE_STATUS);
	
	shader_id = glCreateProgram();
	glAttachShader(shader_id, vertex_shader);
	glAttachShader(shader_id, fragment_shader);
	glLinkProgram(shader_id);
	checkCompileStatus(shader_id, GL_LINK_STATUS);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}
void Shader::use() { glUseProgram(shader_id); }
GLuint Shader::getID() { return shader_id; }
void Shader::setUniform(const std::string & name, GLint value) const { glUniform1i(glGetUniformLocation(shader_id, name.c_str()), value); }
void Shader::setUniform(const std::string & name, GLfloat value) const { glUniform1f(glGetUniformLocation(shader_id, name.c_str()), value); }
void Shader::setUniform(const std::string & name, glm::mat3 value) const { glUniformMatrix3fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
void Shader::setUniform(const std::string & name, glm::mat4 value) const { glUniformMatrix4fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
void Shader::setUniform(const std::string & name, glm::vec3 value) const { glUniform3f(glGetUniformLocation(shader_id, name.c_str()), value.x, value.y, value.z); }