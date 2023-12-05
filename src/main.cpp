#include <core.hpp>
#include <log.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

int
main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    
    if (!init_logging()) {
        std::cerr << "Failed to initialize spdlog. Logging functionality will be limited" << std::endl;
    }
    
    SPDLOG_INFO("Invocation name: \"{}\"", argv[0]);
    for (int i = 1; i < argc; ++i) {
        SPDLOG_INFO("Cmd var. {}: {}", i, argv[i]);
    }
    
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Sparrow", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        // Render here

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
