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

void
glfw_error_callback(int error, const char* description)
{
    UNUSED(error);
    LOG_ERROR("GLFW error: %s", description);
}

static void
test_cglm(void)
{
    LOG_INFO("Testing cglm");
    mat4 m = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    glm_translate(m, (vec3){1.0f, 0.0f, 0.0f});
    LOG_SUCCESS("Didn't crash? Seems like cglm is working");
}

static void
test_truetype(void)
{
    LOG_INFO("Testing truetype");
    const char *font_filename = "res/Roboto-Black.ttf";

    FILE* font_file = fopen(font_filename, "rb");
    fseek(font_file, 0, SEEK_END);
    size_t size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    u32 *font_buffer = (u32 *)malloc(size);

    fread(font_buffer, size, 1, font_file);
    fclose(font_file);

    LOG_INFO("Font file %s loaded into memory.", font_filename);

    /* prepare font */
    stbtt_fontinfo font_info;
    if (!stbtt_InitFont(&font_info, (unsigned char *)font_buffer, 0)) {
        LOG_ERROR("Could not initialize font.");
    } else {
        LOG_INFO("Font %s prepared", font_filename);

        u32 bitmap_width = 512;
        u32 bitmap_height = 128;
        u32 line_height = 64;
        LOG_INFO("Setting up bitmap for text rendering width width = %u, height = %u and line height = %u",
                          bitmap_width, bitmap_height, line_height);
        unsigned char* bitmap = (unsigned char *)calloc(bitmap_width * bitmap_height, sizeof(unsigned char));

        f32 scale = stbtt_ScaleForPixelHeight(&font_info, line_height);

        const char* text = "the quick brown fox";

        u32 x = 0;

        s32 ascent, descent, line_gap;
        stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

        ascent = roundf(ascent * scale);
        descent = roundf(descent * scale);

        LOG_INFO("Font successfully set up.");

        LOG_INFO("Rendering text to bitmap.");
        for (size_t i = 0; i < strlen(text); ++i)
        {
            s32 ax;
            s32 lsb;
            stbtt_GetCodepointHMetrics(&font_info, text[i], &ax, &lsb);
            s32 c_x1, c_y1, c_x2, c_y2;
            stbtt_GetCodepointBitmapBox(&font_info, text[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

            s32 y = ascent + c_y1;

            u32 byte_offset = x + roundf(lsb * scale) + (y * bitmap_width);
            stbtt_MakeCodepointBitmap(&font_info, bitmap + byte_offset, c_x2 - c_x1, c_y2 - c_y1, bitmap_width, scale, scale, text[i]);

            x += roundf(ax * scale);

            s32 kerning;
            kerning = stbtt_GetCodepointKernAdvance(&font_info, text[i], text[i + 1]);
            x += roundf(kerning * scale);
        }
        LOG_INFO("Successfully rendered text.");

        const char *rendered_image_out_file = "build/out.png";
        LOG_INFO("Writing rendered bitmap to file %s.", rendered_image_out_file);
        stbi_write_png("build/out.png", bitmap_width, bitmap_height, 1, bitmap, bitmap_width);
        LOG_INFO("Successfully wrote rendered bitmap to file %s.", rendered_image_out_file);

        free(bitmap);
    }
    free(font_buffer);

    LOG_SUCCESS("Didn't crash? Truetype working");
}

int
main(int argc, const char * argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    LOG_TRACE("Starting application");

    test_cglm();
    test_truetype();

    uint32_t window_width = 1280;
    uint32_t window_height = 720;

    LOG_INFO("Initializing GLFW.");
    b32 glfw_initted = glfwInit();
    if (!glfw_initted) LOG_FATAL("GLFW was not initialized correctly.");
    LOG_SUCCESS("GLFW initialized.");

    LOG_INFO("Setting GLFW error callback function.");
    glfwSetErrorCallback(glfw_error_callback);

    LOG_INFO("Setting GLFW hints to use OpenGL core profile");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    LOG_INFO("Creating window.");
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Sparrow", NULL, NULL);
    if (!window) LOG_FATAL("Could not create window.");
    LOG_SUCCESS("Successfully created window.");

    glfwMakeContextCurrent(window);
    LOG_SUCCESS("Set OpenGL context.");

    // Initialize GLEW
    LOG_INFO("Initializing GLEW");
    GLenum result = glewInit();
    if (result != GLEW_OK) LOG_FATAL("Failed to initialize GLEW");
    LOG_SUCCESS("GLEW successfully initialized");

    // Define vertices
    GLfloat const vertices [] = {
		0.0f, 0.5f,
		0.5f, -0.5f,
		-0.5f, -0.5f
	};

    // Define indices
	GLuint const elements [] = {
		0, 1, 2
	};

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create EBO
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Load vertex shader source
    char *vertex_shader_source;
    size_t vertex_shader_source_size;
    slurp_file("res/shaders/triangle.vert.glsl", &vertex_shader_source, &vertex_shader_source_size);

    // Compile vertex shader
    GLint compiled;
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const char **)&vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		LOG_ERROR("Failed to compile vertex shader!");
	}
    free(vertex_shader_source);

    // Load fragment shader source
    char *fragment_shader_source;
    size_t fragment_shader_source_size;
    slurp_file("res/shaders/triangle.frag.glsl", &fragment_shader_source, &fragment_shader_source_size);

    // Compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const char **)&fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		LOG_ERROR("Failed to compile fragment shader!")
	}
    free(fragment_shader_source);

    // Create shader program
    GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glBindFragDataLocation(shader_program, 0, "outColor");
	glLinkProgram(shader_program);
	glUseProgram(shader_program);

    // Set position attribute for shader program
    GLint position_attribute = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(position_attribute);

    // Set position of attribute in shader
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Delete shader
    glDeleteProgram(shader_program);
	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);

    // Delete buffers
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

    LOG_INFO("Terminating GLFW.");
    glfwTerminate();
    LOG_SUCCESS("GLFW terminated.");

    LOG_SUCCESS("Application terminating with EXIT_SUCCESS exit code.");
    return EXIT_SUCCESS;
}
