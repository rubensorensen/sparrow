#include "log.h"
#include "core.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "stb/stb_image_write.h"
#include "stb/stb_truetype.h"

#include <GLFW/glfw3.h>

void glfw_error_callback(int error, const char* description)
{
    UNUSED(error);
    LOG_ERROR("GLFW error: %s", description);
}

int main(int argc, const char * argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    LOG_TRACE("Starting application");

    LOG_INFO("Testing truetype file loading");
    const char *font_filename = "res/Roboto-Black.ttf";

    FILE* font_file = fopen(font_filename, "rb");
    fseek(font_file, 0, SEEK_END);
    size_t size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    u32 *font_buffer = (u32 *)malloc(size);

    fread(font_buffer, size, 1, font_file);
    fclose(font_file);

    LOG_SUCCESS("Font file %s loaded into memory.", font_filename);

    /* prepare font */
    stbtt_fontinfo font_info;
    if (!stbtt_InitFont(&font_info, (unsigned char *)font_buffer, 0)) {
        LOG_ERROR("Could not initialize font.");
    } else {
        LOG_TRACE("Font %s prepared", font_filename);

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

        LOG_SUCCESS("Font successfully ste up.");

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
        LOG_SUCCESS("Successfully rendered text.");

        const char *rendered_image_out_file = "build/out.png";
        LOG_INFO("Writing rendered bitmap to file %s.", rendered_image_out_file);
        stbi_write_png("build/out.png", bitmap_width, bitmap_height, 1, bitmap, bitmap_width);
        LOG_SUCCESS("Successfully wrote rendered bitmap to file %s.", rendered_image_out_file);

        free(bitmap);
    }
    free(font_buffer);

    uint32_t window_width = 1280;
    uint32_t window_height = 720;

    LOG_INFO("Initializing GLFW.");
    b32 glfw_initted = glfwInit();
    if (!glfw_initted) LOG_FATAL("GLFW was not initialized correctly.");
    LOG_SUCCESS("GLFW initialized.");

    LOG_INFO("Setting GLFW error callback function.");
    glfwSetErrorCallback(glfw_error_callback);

    LOG_INFO("Creating window.");
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Sparrow", NULL, NULL);
    if (!window) LOG_FATAL("Could not create window.");
    LOG_SUCCESS("Successfully created window.");

    glfwMakeContextCurrent(window);
    LOG_SUCCESS("Set OpenGL context.");

    while (!glfwWindowShouldClose(window)) {
        // Render here
        // glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    LOG_INFO("Terminating GLFW.");
    glfwTerminate();
    LOG_SUCCESS("GLFW terminated.");

    LOG_SUCCESS("Application terminating with EXIT_SUCCESS exit code.");
    return EXIT_SUCCESS;
}
