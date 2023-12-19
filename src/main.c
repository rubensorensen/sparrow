#include "log.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "stb/stb_image.h"
#include "stb/stb_truetype.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

typedef struct _Shader {
	u32 id;
} Shader;

typedef struct _Texture {
	u32 id;
} Texture;

typedef enum _CameraMovement {
	CAMERA_MOVEMENT_FORWARD,
	CAMERA_MOVEMENT_BACKWARD,
	CAMERA_MOVEMENT_LEFT,
	CAMERA_MOVEMENT_RIGHT
} CameraMovement;

static const f32 CAMERA_YAW			= -90.0f;
static const f32 CAMERA_PITCH		= 0.0f;
static const f32 CAMERA_SPEED		= 0.1f;
static const f32 CAMERA_SENSITIVITY = 10.0f;
static const f32 CAMERA_ZOOM		= 45.0f;

typedef struct _Camera {
	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	vec3 world_up;

	f32 yaw;
	f32 pitch;

	f32 movement_speed;
	f32 mouse_sensitivity;
	f32 zoom;
} Camera;

static u32 window_width;
static u32 window_height;

static Camera camera;

static f32 delta_time, last_time;

static f32 last_x_pos, last_y_pos;

static b32 first_mouse = true;

static void
update_camera_vectors(Camera *camera)
{
	camera->front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
	camera->front[1] = sinf(glm_rad(camera->pitch));
	camera->front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
	glm_vec3_normalize(camera->front);

	glm_vec3_cross(camera->front, camera->world_up, camera->right);
	glm_vec3_normalize(camera->right);

	glm_vec3_cross(camera->right, camera->front, camera->up);
	glm_vec3_normalize(camera->up);
}

static Camera
create_camera(vec3 position, vec3 up, f32 yaw, f32 pitch)
{
	Camera camera = {0};

	glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.front);
	camera.movement_speed = CAMERA_SPEED;
	camera.mouse_sensitivity = CAMERA_SENSITIVITY;
	camera.zoom = CAMERA_ZOOM;

    glm_vec3_copy(position, camera.position);
	glm_vec3_copy(up, camera.world_up);
	camera.yaw = yaw;
	camera.pitch = pitch;

	update_camera_vectors(&camera);

	return camera;
}

static void
get_camera_view_matrix(Camera *camera, mat4 *result)
{
	vec3 center;
	glm_vec3_add(camera->position, camera->front, center);
	glm_lookat(camera->position, center, camera->up, *result);
}

static void
process_keyboard_for_camera(Camera *camera, CameraMovement direction, f32 delta_time)
{
	f32 velocity = camera->movement_speed * delta_time;

	switch (direction) {
		case CAMERA_MOVEMENT_FORWARD: {
			camera->position[0] += velocity * camera->front[0];
			camera->position[1] += velocity * camera->front[1];
			camera->position[2] += velocity * camera->front[2];
		} break;
		case CAMERA_MOVEMENT_BACKWARD: {
			camera->position[0] -= velocity * camera->front[0];
			camera->position[1] -= velocity * camera->front[1];
			camera->position[2] -= velocity * camera->front[2];
		} break;
		case CAMERA_MOVEMENT_LEFT: {
			vec3 temp;
			glm_vec3_cross(camera->front, camera->up, temp);
			glm_vec3_normalize(temp);

			camera->position[0] -= temp[0] * velocity;
			camera->position[1] -= temp[1] * velocity;
			camera->position[2] -= temp[2] * velocity;
		} break;
		case CAMERA_MOVEMENT_RIGHT: {
			vec3 temp;
			glm_vec3_cross(camera->front, camera->up, temp);
			glm_vec3_normalize(temp);

			camera->position[0] += temp[0] * velocity;
			camera->position[1] += temp[1] * velocity;
			camera->position[2] += temp[2] * velocity;
			
		} break;
	}
}

static void
process_mouse_movement_for_camera(Camera *camera, f32 x_off, f32 y_off,
								  b32 constrain_pitch, f32 delta_time)
{
	f32 speed = camera->mouse_sensitivity * delta_time;
	
	x_off *= speed;
	y_off *= speed;

	camera->yaw += x_off;
	camera->pitch += y_off;

	if (constrain_pitch) {
		if (camera->pitch > 89.0f) {
			camera->pitch = 89.0f;
		} else if (camera->pitch < -89.0f) {
			camera->pitch = -89.0f;
		}
	}

	update_camera_vectors(camera);
}

static void
process_mouse_scroll_for_camera(Camera *camera, f32 y_off)
{
	camera->zoom -= (f32)y_off;
	if (camera->zoom < 1.0f) {
		camera->zoom = 1.0f;
	} else if (camera->zoom > 45.0f) {
		camera->zoom = 45.0f;
	}
}

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

static void
set_shader_m4f32(Shader shader, const char *name, mat4 mat)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.id, name), 1, GL_FALSE, mat[0]);
}

static Texture
create_texture(const char *filepath, b32 flipped)
{
	Texture texture = {0};

	u32 id;
	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	stbi_set_flip_vertically_on_load(flipped);
	s32 width, height, n_channels;
	unsigned char *data = stbi_load(filepath, &width, &height, &n_channels, 4);
	if (!data) {
		LOG_ERROR("Failed to load texture %s", filepath);
		return texture;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	
	texture.id = id;
	return texture;
}

static void
delete_texture(Texture texture)
{
	glDeleteTextures(1, &texture.id);
}

static void
use_texture(Texture texture, s32 unit)
{
	glActiveTexture(unit);
	glBindTexture(GL_TEXTURE_2D, texture.id);
}

static void
glfw_error_callback(int error, const char* description)
{
    UNUSED(error);
    LOG_ERROR("GLFW error: %s", description);
}

static void
cursor_position_callback(GLFWwindow *window, f64 x_pos_, f64 y_pos_)
{
	UNUSED(window);
	
	f32 x_pos = (f32)x_pos_;
	f32 y_pos = (f32)y_pos_;

	if (first_mouse) {
		last_x_pos = x_pos;
		last_y_pos = y_pos;
		first_mouse = false;
	}

	f32 x_off = x_pos - last_x_pos;
	f32 y_off = last_y_pos - y_pos;

	last_x_pos = x_pos;
	last_y_pos = y_pos;

	process_mouse_movement_for_camera(&camera, x_off, y_off, true, delta_time);
}

static void
scroll_callback(GLFWwindow *window, f64 x_offset, f64 y_offset)
{
	UNUSED(window);
	UNUSED(x_offset);
	
	process_mouse_scroll_for_camera(&camera, (f32)y_offset);
}
	
static void
glfw_framebuffer_size_callback(GLFWwindow *window, s32 width, s32 height)
{
	UNUSED(window);

	window_width = width;
	window_height = height;

	last_x_pos = (f32)width/2.0f;
	last_y_pos = (f32)height/2.0f;
	
	glViewport(0, 0, width, height);
}

static void
process_inputs(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		process_keyboard_for_camera(&camera, CAMERA_MOVEMENT_FORWARD, true);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		process_keyboard_for_camera(&camera, CAMERA_MOVEMENT_BACKWARD, true);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		process_keyboard_for_camera(&camera, CAMERA_MOVEMENT_LEFT, true);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		process_keyboard_for_camera(&camera, CAMERA_MOVEMENT_RIGHT, true);
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
	
	last_x_pos = (f32)window_width/2.0f;
	last_y_pos = (f32)window_height/2.0f;
	
    LOG_INFO("Creating window.");
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Sparrow", NULL, NULL);
    if (!window) {
		glfwTerminate();
		LOG_FATAL("Could not create window");
	}
    LOG_SUCCESS("Successfully created window");

    glfwMakeContextCurrent(window);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

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

	/* float vertices[] = { */
    /*     // positions            // texture coords */
	/* 	 0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // top right */
	/* 	 0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right */
    /*     -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // bottom left */
    /*     -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // top left  */
    /* }; */
	
    /* unsigned int indices[] = {   */
    /*     0, 1, 3, // first triangle */
    /*     1, 2, 3  // second triangle */
    /* }; */
	
	float vertices[] = {
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

	vec3 cube_positions[] = {
		{ 0.0f,  0.0f,  0.0f}, 
		{ 2.0f,  5.0f, -15.0f}, 
		{-1.5f, -2.2f, -2.5f},  
		{-3.8f, -2.0f, -12.3f},  
		{ 2.4f, -0.4f, -3.5f},  
		{-1.7f,  3.0f, -7.5f},  
		{ 1.3f, -2.0f, -2.5f},  
		{ 1.5f,  2.0f, -2.5f}, 
		{ 1.5f,  0.2f, -1.5f}, 
		{-1.3f,  1.0f, -1.5f}
	};
	
    unsigned int vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); */
    /* glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); */

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
	
	Texture texture1 = create_texture("res/wall.jpg", false);
	Texture texture2 = create_texture("res/awesomeface.png", true);
	
	use_shader(shader);
	set_shader_s32(shader, "texture1", 0);
	set_shader_s32(shader, "texture2", 1);

	glEnable(GL_DEPTH_TEST);

	camera = create_camera((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, 0.0f);
	
	// Main loop
	LOG_INFO("Entering main loop")
    while (!glfwWindowShouldClose(window)) {
		f32 current_time = glfwGetTime();
		delta_time = current_time - last_time;
		last_time = current_time;
		
		process_inputs(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    
		use_texture(texture1, GL_TEXTURE0);
		use_texture(texture2, GL_TEXTURE1);
		
		use_shader(shader);
		
		mat4 projection;
		glm_perspective(glm_rad(camera.zoom), (float)window_width/(float)window_height,
						0.1f, 100.0f, projection);
		set_shader_m4f32(shader, "projection", projection);

		mat4 view;
		get_camera_view_matrix(&camera, &view);
		set_shader_m4f32(shader, "view", view);
	   				
		glBindVertexArray(vao);
		/* glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); */

		for (u32 i = 0; i < 10; ++i) {
			mat4 model = GLM_MAT4_IDENTITY_INIT;
			glm_translate(model, cube_positions[i]);
			float angle = 20.0f * (i+1) * glfwGetTime();
			glm_rotate(model, glm_rad(angle), (vec3){1.0f, 0.3f, 0.5f});
			set_shader_m4f32(shader, "model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}		
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	LOG_INFO("Exiting main loop");

	LOG_INFO("Cleaning up OpenGL resources");
	delete_texture(texture1);
	delete_texture(texture2);
	
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	
	delete_shader(shader);
	
    LOG_INFO("Terminating GLFW.");
    glfwTerminate();
    LOG_SUCCESS("GLFW terminated.");

    LOG_SUCCESS("Application terminating with EXIT_SUCCESS exit code.");
    return EXIT_SUCCESS;
}
