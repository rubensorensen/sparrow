#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include "stb_image_write.h"
#include "stb_truetype.h"

#include <GLFW/glfw3.h>

#define UNUSED(x) ((void)x)

int main(int argc, const char * argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    FILE* font_file = fopen("res/Roboto-Black.ttf", "rb");
    fseek(font_file, 0, SEEK_END);
    size_t size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    uint32_t *font_buffer = (uint32_t *)malloc(size);

    fread(font_buffer, size, 1, font_file);
    fclose(font_file);

    /* prepare font */
    stbtt_fontinfo font_info;
    if (!stbtt_InitFont(&font_info, (unsigned char *)font_buffer, 0))
    {
        printf("failed\n");
    }

    int bitmap_width = 512;
    int bitmap_height = 128;
    int line_height = 64;

    unsigned char* bitmap = (unsigned char *)calloc(bitmap_width * bitmap_height, sizeof(unsigned char));

    float scale = stbtt_ScaleForPixelHeight(&font_info, line_height);

    const char* text = "the quick brown fox";

    int x = 0;

    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    for (size_t i = 0; i < strlen(text); ++i)
    {
        int ax;
        int lsb;
        stbtt_GetCodepointHMetrics(&font_info, text[i], &ax, &lsb);
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&font_info, text[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        int y = ascent + c_y1;

        int byte_offset = x + roundf(lsb * scale) + (y * bitmap_width);
        stbtt_MakeCodepointBitmap(&font_info, bitmap + byte_offset, c_x2 - c_x1, c_y2 - c_y1, bitmap_width, scale, scale, text[i]);

        x += roundf(ax * scale);

        int kerning;
        kerning = stbtt_GetCodepointKernAdvance(&font_info, text[i], text[i + 1]);
        x += roundf(kerning * scale);
    }

    stbi_write_png("build/out.png", bitmap_width, bitmap_height, 1, bitmap, bitmap_width);

    free(font_buffer);
    free(bitmap);

    uint32_t window_width = 1280;
    uint32_t window_height = 720;

    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "Sparrow", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Render here
        // glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
