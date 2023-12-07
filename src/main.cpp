#include <core.hpp>
#include <log.hpp>

#include <SDL3/SDL.h>

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
    
    LOG_INFO("Invocation name: \"{}\"", argv[0]);
    for (int i = 1; i < argc; ++i) {
        LOG_INFO("Cmd var. {}: {}", i, argv[i]);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        LOG_ERROR("SDL_Init failed: {}", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Sparrow", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        LOG_ERROR("SDL_CreateWindow failed: {}", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    bool should_quit = false;
    while (!should_quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                should_quit = true;
            }
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();
    
    return EXIT_SUCCESS;
}
