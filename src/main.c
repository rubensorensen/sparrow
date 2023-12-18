#include "log.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include "stb/stb_image.h"
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

typedef struct _Shader {
	u32 id;
} Shader;

static Shader
create_shader(const char *vertex_filepath, const char *fragment_filepath)
{
	Shader shader = {0};

	u32 vertex_shader, fragment_shader;
	
	b32 success;
	char buf[512];
	
	// Vertex shader
	char *vertex_shader_source;
	size_t vertex_shader_source_size;
	success = slurp_file(vertex_filepath,
						&vertex_shader_source, &vertex_shader_source_size);
	if (!success) {
		LOG_WARNING("Shader source file %s was not loaded", vertex_filepath);
	}
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const char * const *)&vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, buf);
		LOG_ERROR("Vertex shader compilation failed: %s", buf);
	}
	free(vertex_shader_source);

	// Fragment shader
	char *fragment_shader_source;
	size_t fragment_shader_source_size;
	success = slurp_file(fragment_filepath,
						&fragment_shader_source, &fragment_shader_source_size);
	if (!success) {
		LOG_WARNING("Shader source file %s was not loaded", fragment_filepath);
	}
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const char * const *)&fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, buf);
		LOG_ERROR("Fragment shader compilation failed: %s", buf);
	}
	free(fragment_shader_source);	    

	// Program
	u32 program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, buf);
		LOG_ERROR("Shader program linking  failed: %s", buf);
	}
	glUseProgram(program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	shader.id = program;
	return shader;
}

static void
delete_shader(Shader shader) {
	glDeleteShader(shader.id);
}

static void
use_shader(Shader shader)
{
	glUseProgram(shader.id);
}

static void
set_shader_b32(Shader shader, const char *name, b32 value)
{
	glUniform1i(glGetUniformLocation(shader.id, name), (int)value);
}

static void
set_shader_s32(Shader shader, const char *name, s32 value)
{
	glUniform1i(glGetUniformLocation(shader.id, name), value);
}

static void
set_shader_f32(Shader shader, const char *name, f32 value)
{
	glUniform1f(glGetUniformLocation(shader.id, name), value);
}

static void
set_shader_v4f32(Shader shader, const char *name, f32 v1, f32 v2, f32 v3, f32 v4)
{
	glUniform4f(glGetUniformLocation(shader.id, name), v1, v2, v3, v4);
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

	Shader shader = create_shader("res/shaders/triangle.vert.glsl",
								  "res/shaders/triangle.frag.glsl");

	float vertices[] = {
        // positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
	
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
	
	stbi_set_flip_vertically_on_load(true);  

    u32 texture1;
    {
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

		int width, height, n_channels;
		unsigned char *data = stbi_load("res/wall.jpg", &width, &height, &n_channels, 0);
		if (!data) {
			LOG_ERROR("Failed to load texture");
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}

	u32 texture2;
    {
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

		int width, height, n_channels;
		unsigned char *data = stbi_load("res/awesomeface.png", &width, &height, &n_channels, 0);
		if (!data) {
			LOG_ERROR("Failed to load texture");
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}

	use_shader(shader);
	set_shader_s32(shader, "texture1", 0);
	set_shader_s32(shader, "texture2", 1);
	
	// Main loop
	LOG_INFO("Entering main loop")
    while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		use_shader(shader);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	LOG_INFO("Exiting main loop");

	LOG_INFO("Cleaning up OpenGL resources");
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	delete_shader(shader);
	
    LOG_INFO("Terminating GLFW.");
    glfwTerminate();
    LOG_SUCCESS("GLFW terminated.");

    LOG_SUCCESS("Application terminating with EXIT_SUCCESS exit code.");
    return EXIT_SUCCESS;
}
