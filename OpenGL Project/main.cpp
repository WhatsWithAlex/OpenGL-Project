#include <iostream>
#include <vector>
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

#define SHDW_MAP_WIDTH 2048
#define SHDW_MAP_HEIGHT 2048

GLfloat current_time = 0.0f, last_time = 0.0f, frame_time, time_scale = 1.0f;

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

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), false);
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
	
	camera.processKeyboard(window, time_scale * frame_time);
}

GLuint loadSkyBox(std::vector <string> textures) 
{
	GLuint id;
	GLubyte *data;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	GLint width, height, nr_channels;
	for (int i = 0; i < textures.size(); ++i)
	{
		data = stbi_load(textures[i].c_str(), &width, &height, &nr_channels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			std::cout << "Error while loading skybox!\n";
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return id;
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

	// Глобальные настройки
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Буффер и карта глубины
	GLuint depth_map_buffer;
	glGenFramebuffers(1, &depth_map_buffer);

	GLuint depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHDW_MAP_WIDTH, SHDW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_buffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Компиляция шейдеров
	Shader shader("vertex.vsh", "fragment.fsh"), light_shader("vertex_light.vsh", "fragment_light.fsh"), depth_shader("vertex_depth.vsh", "fragment_depth.fsh");
	Shader sky_shader("vertex_sky.vsh", "fragment_sky.fsh");

	// Загрузка скайбокса
	std::vector <string> textures = {
		"Textures/skybox/skybox_RT.jpg", "Textures/skybox/skybox_LF.jpg",
		"Textures/skybox/skybox_UP.jpg", "Textures/skybox/skybox_DN.jpg",
		"Textures/skybox/skybox_FR.jpg", "Textures/skybox/skybox_BK.jpg",
	};
	GLuint skybox = loadSkyBox(textures);

	// Загрузка моделей
	Model myearth("Models/earth.obj"), moon("Models/moon.obj"), box("Models/wall.obj"), skycube("Models/cube.obj");

	// Создание источников света
	DirectedLight dir_light = {
		glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::vec3(0.03f, 0.02f, 0.01f),
		glm::vec3(0.8f, 0.6f, 0.6f),
		glm::vec3(1.0f, 0.5f, 0.0f)
	};
	PointLight point_light = {
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(0.03f, 0.01f, 0.00f),
		glm::vec3(0.8f, 0.05f, 0.0f),
		glm::vec3(1.0f, 0.1f, 0.0f),
		1.0f, 0.14f, 0.07f
	};

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

	// Матрицы перехода
	glm::mat4 projection, light_projection, view, light_view, light_space, model, moon_model;
	projection = glm::perspective(glm::radians(50.0f), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
	light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
	light_view = glm::lookAt(-dir_light.dir * glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f), global_up);
	light_space = light_projection * light_view;

	// Настройка шейдеров
	depth_shader.use();
	depth_shader.setUniform("light_space", light_space);

	shader.use();
	shader.setUniform("projection", projection);
	shader.setUniform("light_space", light_space);
	shader.setUniform("material.shininess", 64.0f);
	shader.setUniform("dir_light.dir", dir_light.dir);
	shader.setUniform("dir_light.ambient_intensity", dir_light.ambient_intensity);
	shader.setUniform("dir_light.diffuse_intensity", dir_light.diffuse_intensity);
	shader.setUniform("dir_light.specular_intensity", dir_light.specular_intensity);
	/*shader.setUniform("point_light[0].ambient_intensity", point_light.ambient_intensity);
	shader.setUniform("point_light[0].diffuse_intensity", point_light.diffuse_intensity);
	shader.setUniform("point_light[0].specular_intensity", point_light.specular_intensity);
	shader.setUniform("point_light[0].constant", point_light.constant);
	shader.setUniform("point_light[0].linear", point_light.linear);
	shader.setUniform("point_light[0].quadratic", point_light.quadratic);*/

	light_shader.use();
	light_shader.setUniform("projection", projection);
	light_shader.setUniform("light_color", point_light.specular_intensity);

	sky_shader.use();
	sky_shader.setUniform("projection", projection);
	sky_shader.setUniform("skybox", 16);

	// Основной цикл рендеринга
	while (!glfwWindowShouldClose(window))
	{
		current_time = glfwGetTime();
		frame_time = current_time - last_time;
		last_time = current_time;
		
		processInputEvents(window);

		glClearColor(0.0f, 0.01f, 0.03f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//point_light.pos = glm::vec3(2.5f, 1.0f * sin(time_scale * current_time / 2), 1.0f * cos(time_scale * current_time / 2));
		GLfloat T = time_scale * current_time;

		view = camera.getLookAt();
		
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		model = glm::rotate(model, time_scale * current_time / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		moon_model = glm::mat4(1.0f);
		moon_model = glm::translate(moon_model, glm::vec3(3.0 * sin(T), 0.0f, 5.0 * cos(T)));
		moon_model = glm::scale(moon_model, glm::vec3(1.0f, 1.0f, 1.0f));

		// Рендеринг в карту глубины
		glCullFace(GL_FRONT);
		glViewport(0, 0, SHDW_MAP_WIDTH, SHDW_MAP_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_buffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		depth_shader.use();
		depth_shader.setUniform("model", model);
		depth_shader.setUniform("view", view);

		myearth.render(depth_shader);
		depth_shader.setUniform("model", moon_model);
		moon.render(depth_shader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// Рендеринг в стандартный буфер
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_2D, depth_map);
		shader.use();
		shader.setUniform("view", view);
		shader.setUniform("model", model);
		shader.setUniform("shadow_map", 15);

		myearth.render(shader);
		shader.setUniform("model", moon_model);
		moon.render(shader);

		glDepthFunc(GL_LEQUAL);
		glm::mat4 sky_view = glm::mat4(glm::mat3(view));
		sky_shader.use();
		sky_shader.setUniform("view", sky_view);
		sky_shader.setUniform("projection", projection);

		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
		skycube.render(sky_shader);
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}