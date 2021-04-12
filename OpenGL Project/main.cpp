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
#include "texture.h"
#include "model.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 800

GLfloat current_time = 0.0f, last_time = 0.0f, frame_time;

struct Material 
{
	Texture2D diffuse_map;
	Texture2D specular_map;
	Texture2D normal_map;
	GLfloat shininess;
};
struct DirectedLight 
{
	glm::vec3 dir;
	glm::vec3 ambient_intensity;
	glm::vec3 diffuse_intensity;
	glm::vec3 specular_intensity;
};
struct PointLight
{
	glm::vec3 pos;
	glm::vec3 ambient_intensity;
	glm::vec3 diffuse_intensity;
	glm::vec3 specular_intensity;

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
};

void framebufferSizeCallback(GLFWwindow *window, GLint width, GLint height) 
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
		last_x = (GLfloat)xpos;
		last_y = (GLfloat)ypos;
		first = false;
	}
	GLfloat offset_x = xpos - last_x;
	GLfloat offset_y = ypos - last_y;
	last_x = xpos;
	last_y = ypos;

	camera.processMouse(offset_x, offset_y);
}
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		camera.changeLock();
}
void processInputEvents(GLFWwindow *window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	camera.processKeyboard(window, frame_time);
}

GLFWwindow *initGLFWWindow(GLuint width, GLuint height) 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Version 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL Program", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Компиляция шейдеров
	Shader shader("vertex.vsh", "fragment.fsh"), light_shader("vertex_light.vsh", "fragment_light.fsh");
	
	// Загрузка текстур
	Texture2D diffuse_map("Textures/wall/wall_diffuse.jpg", "diffuse_map");
	Texture2D specular_map("Textures/wall/wall_specular.jpg", "specular_map");
	Texture2D normal_map("Textures/wall/wall_normal.jpg", "normal_map");

	// Создание материала
	Material material = {
		diffuse_map, 
		specular_map,
		normal_map,
		32.0f
	};
	
	Model mycube("Models/cube.obj");

	// Создание источников света
	DirectedLight dir_light = {
		glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f)),
		glm::vec3(0.02f, 0.02f, 0.03f),
		glm::vec3(0.8f, 0.7f, 0.8f),
		glm::vec3(0.9f, 0.9f, 1.0f)
	};
	PointLight point_light = {
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(0.03f, 0.01f, 0.00f),
		glm::vec3(0.8f, 0.05f, 0.0f),
		glm::vec3(1.0f, 0.1f, 0.0f),
		1.0f, 0.14f, 0.07f
	};

	GLfloat T = 0.0f;
	GLfloat cube[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	};
	GLfloat pyramid[] = {
		 0.0f,  0.3f,  0.0f,  0.5f, 1.0f,  0.0f,  0.53f,  -0.848f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.53f,  -0.848f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.53f,  -0.848f,

		 0.0f,  0.3f,  0.0f,  0.5f, 1.0f,  0.848f, 0.53f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.848f, 0.53f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.848f, 0.53f,  0.0f,

		 0.0f,  0.3f,  0.0f,  0.5f, 1.0f,  0.0f,  0.53f,  0.848f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.53f,  0.848f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.53f,  0.848f,

		 0.0f,  0.3f,  0.0f,  0.5f, 1.0f, -0.848f,  0.53f,  0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, -0.848f,  0.53f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -0.848f,  0.53f,  0.0f,

		-0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	};

	glm::vec3 object_positions[] = {
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
	GLuint vertex_array, light_vertex_array, vertex_buffer, light_vertex_buffer, element_buffer;

	// Создание объектов буфера и массива вершин и буфера элементов
	glGenVertexArrays(1, &vertex_array);
	glGenVertexArrays(1, &light_vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &light_vertex_buffer);
	glGenBuffers(1, &element_buffer);

	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid), pyramid, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	glBindVertexArray(light_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, light_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 projection, view;
	projection = glm::perspective(glm::radians(50.0f), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat3 normal_matrix;

	shader.use();
	shader.setUniform("projection", projection);
	//shader.setUniform("material.diffuse_map[0]", 0);
	shader.setUniform("material.specular_map[0]", 1);
	shader.setUniform("material.normal_map[0]", 2);
	shader.setUniform("material.shininess", material.shininess);
	shader.setUniform("dir_light.dir", dir_light.dir);
	shader.setUniform("dir_light.ambient_intensity", dir_light.ambient_intensity);
	shader.setUniform("dir_light.diffuse_intensity", dir_light.diffuse_intensity);
	shader.setUniform("dir_light.specular_intensity", dir_light.specular_intensity);
	shader.setUniform("point_light[0].ambient_intensity", point_light.ambient_intensity);
	shader.setUniform("point_light[0].diffuse_intensity", point_light.diffuse_intensity);
	shader.setUniform("point_light[0].specular_intensity", point_light.specular_intensity);
	shader.setUniform("point_light[0].constant", point_light.constant);
	shader.setUniform("point_light[0].linear", point_light.linear);
	shader.setUniform("point_light[0].quadratic", point_light.quadratic);

	//material.diffuse_map.active(GL_TEXTURE0);
	material.specular_map.active(GL_TEXTURE1);
	material.normal_map.active(GL_TEXTURE2);

	light_shader.use();
	light_shader.setUniform("light_color", point_light.specular_intensity);
	light_shader.setUniform("projection", projection);


	// Основной цикл рендеринга
	while (!glfwWindowShouldClose(window))
	{
		current_time = glfwGetTime();
		frame_time = current_time - last_time;
		last_time = current_time;
		
		processInputEvents(window);

		T += 0.05f;
		glClearColor(0.0f, 0.01f, 0.03f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		point_light.pos = glm::vec3(1.5f, 1.0f * sin(T / 2), 1.0f * cos(T / 2));
		// Объекты
		shader.use();
		shader.setUniform("point_light[0].pos", point_light.pos);

		view = camera.getLookAt();
		shader.setUniform("view", view);
		
		model = glm::mat4(1.0f);
		model = glm::rotate(model, T / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setUniform("model", model);
		normal_matrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
		shader.setUniform("normal_matrix", normal_matrix);

		/*glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 18);*/
		mycube.render(shader);

		light_shader.use();
		light_shader.setUniform("view", camera.getLookAt());
		model = glm::mat4(1.0f);
		model = glm::translate(model, point_light.pos);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		light_shader.setUniform("model", model);

		glBindVertexArray(light_vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);

	glfwTerminate();
	return 0;
}