#pragma once

#include <iostream>
#include <string>
#include <glad/glad.h>
#include "stb_image.h"

class Texture2D 
{
	GLuint id;
	GLint width, height, nr_channels;
	GLubyte *data;
	bool gen_mipmap;
public:
	Texture2D(std::string path, GLint par1, GLint par2, GLint par3, GLint par4, bool gen_mipmap);
	void load(std::string path);
	void setParameter(GLint parameter, GLint value);
	void bind();
	void unbind();
	void active(GLint slot);
};

Texture2D::Texture2D(std::string path, GLint par1 = GL_REPEAT, GLint par2 = GL_REPEAT, GLint par3 = GL_LINEAR_MIPMAP_LINEAR, GLint par4 = GL_LINEAR, bool gen_mipmap = true) 
	: gen_mipmap(gen_mipmap)
{
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
	if (data == nullptr)
	{
		std::cout << "Texture loading error\n";
		throw -1;
	}
	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, par1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, par2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, par3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, par4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	if (gen_mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture2D::load(std::string path)
{
	data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
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