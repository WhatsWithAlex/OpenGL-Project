#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>
#include "stb_image.h"

class Texture2D 
{
	GLuint id;
	std::string type;
public:
	Texture2D(const std::string &filename, const std::string &type, const std::string &directory, GLint par1, GLint par2, GLint par3, GLint par4, bool gen_mipmap);
	GLuint getID();
	std::string getType();
	void load(std::string path, bool gen_mipmap);
	void setParameter(GLint parameter, GLint value);
	void bind();
	void unbind();
	void active(GLint slot);
};

Texture2D::Texture2D(const std::string &filename, const std::string &type, const std::string &directory = ".",
	GLint par1 = GL_REPEAT, GLint par2 = GL_REPEAT, GLint par3 = GL_LINEAR_MIPMAP_LINEAR, GLint par4 = GL_LINEAR, bool gen_mipmap = true) 
	: type(type)
{
	
	stbi_set_flip_vertically_on_load(true);
	std::string path = directory + "/" + filename ;
	GLint width, height, nr_channels;
	GLenum channels = GL_RED;
	GLubyte *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
	if (data == nullptr)
	{
		std::cout << "Texture loading error. Path: " << path << std::endl;
		throw -1;
	}
	
	if (nr_channels == 1)
		channels = GL_RED;
	else if (nr_channels == 3)
		channels = GL_RGB;
	else if (nr_channels == 4)
		channels = GL_RGBA;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, par1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, par2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, par3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, par4);
	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
	if (gen_mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}
GLuint Texture2D::getID() { return id; }
std::string Texture2D::getType() { return type; }
void Texture2D::load(std::string path, bool gen_mipmap)
{
	GLint width, height, nr_channels;
	GLubyte *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
	if (data == nullptr)
	{
		std::cout << "Texture loading error\n";
		throw - 1;
	}
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	if (gen_mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture2D::setParameter(GLint parameter, GLint value) {
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, parameter, value);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture2D::bind() { glBindTexture(GL_TEXTURE_2D, id); }
void Texture2D::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
void Texture2D::active(GLint slot) 
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, id);
}