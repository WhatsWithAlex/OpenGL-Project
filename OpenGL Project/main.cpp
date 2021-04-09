#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "shader.h"
#include "camera.h"

#define SCR_WIDTH 500
#define SCR_HEIGHT 500

void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height) 
{
	glViewport(0, 0, width, height);
}

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), false);
GLfloat last_x, last_y;
bool first = true;
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	
	if (first) 
	{
		last_x = SCR_WIDTH / 2;
		last_y = SCR_HEIGHT / 2;
		first = false;
	}
	GLfloat offset_x = xpos - last_x;
	GLfloat offset_y = ypos - last_y;
	last_x = xpos;
	last_y = ypos;
	
	glm::vec3 axis = -glm::vec3(offset_y, offset_x, 0.0f);
	GLfloat angle = glm::length(axis);
	camera.rotate(camera.sensitivity * glm::radians(angle), glm::normalize(axis));
}

void processInputEvents(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) 
	{
		glClearColor(1.0, 1.0, 1.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
	}
}

GLFWwindow* initGLFWWindow(GLuint width, GLuint height) 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Version 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Program", NULL, NULL);
	if (window == nullptr) 
	{
		std::cout << "Error while creating window!\n";
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Error initializing GLAD!\n";
		return nullptr;
	}
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

int main() 
{
	// Инициализация окна
	GLFWwindow* window = initGLFWWindow(SCR_WIDTH, SCR_HEIGHT);
	if (window == nullptr) 
	{
		std::cout << "Initialization failed\n";
		return -1;
	}

	// Компиляция шейдеров
	Shader shader("vertex.vsh", "fragment.fsh");

	
	GLint width, height, nr_channels;
	GLubyte* texture_data;
	GLuint texture1, texture2;

	stbi_set_flip_vertically_on_load(true);

	// Загрузка текстуры 1
	texture_data = stbi_load("Textures/texture.jpg", &width, &height, &nr_channels, 0);
	if (texture_data == nullptr) 
	{
		std::cout << "Texture loading error\n";
		return -1;
	}

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texture_data);

	// Загрузка текстуры 2
	texture_data = stbi_load("Textures/texture.jpg", &width, &height, &nr_channels, 0);
	if (texture_data == nullptr) 
	{
		std::cout << "Texture loading error\n";
		return -1;
	}

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texture_data);

	GLfloat R = 0.5, G = 0.5, B = 0.5, T = 0.0;
	GLfloat rectangle[] = {
		-0.5, -0.5, 0.0,  1.0, 0.0, 0.0,  0.0, 0.0,
		 0.5, -0.5, 0.0,  0.0, 1.0, 0.0,  1.0, 0.0,
		-0.5,  0.5, 0.0,  0.0, 0.0, 1.0,  0.0, 1.0,
		 0.5,  0.5, 0.0,  1.0, 1.0, 0.0,  1.0, 1.0
	};
	GLfloat cube[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	//-0.5, -0.5,  0.5,  0.0, 0.0,
	// 0.5, -0.5,  0.5,  0.0, 0.0,
	GLfloat pyramid[] = {
		 0.0,  0.2,  0.0,  0.3, 1.0,
		-0.5, -0.5, -0.5,  1.0, 0.0,
		 0.5, -0.5, -0.5,  0.0, 0.0,

		 0.0,  0.2,  0.0,  0.3, 1.0,
		 0.5, -0.5, -0.5,  1.0, 0.0,
		 0.5, -0.5,  0.5,  0.0, 0.0,

		 0.0,  0.2,  0.0,  0.3, 1.0,
		 0.5, -0.5,  0.5,  1.0, 0.0,
		-0.5, -0.5,  0.5,  0.0, 0.0,

		 0.0,  0.2,  0.0,  0.3, 1.0,
		-0.5, -0.5,  0.5,  1.0, 0.0,
		-0.5, -0.5, -0.5,  0.0, 0.0,

		-0.5, -0.5, -0.5,  1.0, 1.0,
		 0.5, -0.5, -0.5,  1.0, 0.0,
		 0.5, -0.5,  0.5,  0.0, 1.0,
		 0.5, -0.5,  0.5,  0.0, 1.0,
		-0.5, -0.5,  0.5,  0.0, 0.0,
		-0.5, -0.5, -0.5,  1.0, 1.0
	};
	glm::vec3 cube_positions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	GLuint vertex_array, vertex_buffer, element_buffer;

	// Создание объектов буфера и массива вершин и буфера элементов
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);

	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid), pyramid, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader.use();
	shader.setUniform("texture1", 0);
	shader.setUniform("texture2", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	
	shader.setUniform("projection", projection);
	glEnable(GL_DEPTH_TEST);

	// Основной цикл рендеринга
	while (!glfwWindowShouldClose(window))
	{
		processInputEvents(window);
		camera.processKeyboard(window);

		T += 0.05;
		glClearColor(0.54, 0.78, 1.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera.setPos(glm::vec3(5.0 * cos(-T), 0.0, 5.0 * sin(-T)));
		//camera.rotate(0.05, glm::vec3(0.0, 1.0, 0.0));
		shader.use();
		shader.setUniform("view", camera.getLookAt());
		
		glBindVertexArray(vertex_array);
		for (int i = 0; i < 1; ++i) 
		{
			glm::mat4 model(1.0);
			model = glm::translate(model, cube_positions[i]);
			model = glm::rotate(model, T, glm::vec3(0.0, 1.0, 0.0));
			shader.setUniform("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 18);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);

	glfwTerminate();
	return 0;
}