#include "log.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include "stb/stb_image_write.h"
#include "stb/stb_truetype.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

static u32 window_width;
static u32 window_height;

static void
glfw_error_callback(int error, const char* description)
{
    UNUSED(error);
    LOG_ERROR("GLFW error: %s", description);
}

static void
glfw_framebuffer_size_callback(GLFWwindow *window, s32 width, s32 height)
{
	UNUSED(window);

	window_width = width;
	window_height = height;
	
	glViewport(0, 0, width, height);
}

static void
process_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int
main(int argc, const char * argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    LOG_TRACE("Starting application");

    LOG_INFO("Initializing GLFW.");
    b32 glfw_initted = glfwInit();
    if (!glfw_initted) {
		LOG_FATAL("GLFW was not initialized correctly");
	}
    LOG_SUCCESS("GLFW initialized");

    LOG_INFO("Setting GLFW error callback function");
    glfwSetErrorCallback(glfw_error_callback);

    LOG_INFO("Setting GLFW hints to use OpenGL core profile");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	window_width = 1280;
	window_height = 720;
    LOG_INFO("Creating window.");
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Sparrow", NULL, NULL);
    if (!window) {
		glfwTerminate();
		LOG_FATAL("Could not create window");
	}
    LOG_SUCCESS("Successfully created window");

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    LOG_INFO("Initializing GLEW");
    if (glewInit() != GLEW_OK) {
		glfwTerminate();
		LOG_FATAL("Failed to initialize GLEW");
	}
    LOG_SUCCESS("GLEW successfully initialized");

	glViewport(0, 0, window_width, window_height);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);


	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	
	u32 vbo;
	glGenBuffers(1, &vbo);
	
	u32 vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex shader
	u32 vertex_shader;
	{
		const char *vertex_shader_source_filename = "res/shaders/triangle.vert.glsl";
		char *vertex_shader_source;
		size_t vertex_shader_source_size;
		b32 result = slurp_file(vertex_shader_source_filename,
								&vertex_shader_source, &vertex_shader_source_size);
		if (!result) {
			LOG_WARNING("Shader source file %s was not loaded", vertex_shader_source_filename);
		}
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, (const char * const *)&vertex_shader_source, NULL);
		glCompileShader(vertex_shader);
		b32 success;
		char info_log[512];
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
			LOG_ERROR("Vertex shader compilation failed: %s", info_log);
		}
		free(vertex_shader_source);
	}

	// Fragment shader
	u32 fragment_shader;
	{
		const char *fragment_shader_source_filename = "res/shaders/triangle.frag.glsl";
		char *fragment_shader_source;
		size_t fragment_shader_source_size;
		b32 result = slurp_file(fragment_shader_source_filename,
								&fragment_shader_source, &fragment_shader_source_size);
		if (!result) {
			LOG_WARNING("Shader source file %s was not loaded", fragment_shader_source_filename);
		}
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, (const char * const *)&fragment_shader_source, NULL);
		glCompileShader(fragment_shader);
		b32 success;
		char info_log[512];
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
			LOG_ERROR("Fragment shader compilation failed: %s", info_log);
		}
		free(fragment_shader_source);		
	}

	// Shader program
	u32 shader_program;
	{
		shader_program = glCreateProgram();
		glAttachShader(shader_program, vertex_shader);
		glAttachShader(shader_program, fragment_shader);
		glLinkProgram(shader_program);
		b32 success;
		char info_log[512];
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader_program, 512, NULL, info_log);
			LOG_ERROR("Shader program linking  failed: %s", info_log);
		}
		glUseProgram(shader_program);
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	
	// Shader attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	
    // Main loop
	LOG_INFO("Entering main loop")
    while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		glUseProgram(shader_program);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	LOG_INFO("Exiting main loop");
	
    LOG_INFO("Terminating GLFW.");
    glfwTerminate();
    LOG_SUCCESS("GLFW terminated.");

    LOG_SUCCESS("Application terminating with EXIT_SUCCESS exit code.");
    return EXIT_SUCCESS;
}
