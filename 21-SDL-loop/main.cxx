#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include <SDL3/SDL.h>

struct Bind
{
    SDL_Keycode      key;
    std::string_view name;
};

std::array<Bind, 8> key_bindings = { { { SDLK_w, "Up" },
                                       { SDLK_s, "Down" },
                                       { SDLK_a, "Left" },
                                       { SDLK_d, "Right" },
                                       { SDLK_1, "Button 1" },
                                       { SDLK_2, "Button 2" },
                                       { SDLK_e, "Select" },
                                       { SDLK_SPACE, "Start" } } };

void CheckPressingKey(SDL_Event event)
{
    const auto it = std::find_if(key_bindings.begin(),
                                 key_bindings.end(),
                                 [&](const ::Bind& b)
                                 { return b.key == event.key.keysym.sym; });
    if (it != key_bindings.end())
    {
        std::cout << it->name << " ";
        if (event.type == SDL_EVENT_KEY_UP)
            std::cout << "is realesed" << std::endl;
        else
            std::cout << "is pressed" << std::endl;
    }
}

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL_INIT error : " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("SDL Loop", 800, 600, 0);
    if (!window)
    {
        std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Event event;
    bool      quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        quit = true;
                        break;
                    }
                    [[fallthrough]];
                case SDL_EVENT_KEY_UP:
                    CheckPressingKey(event);
                    break;
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();
}
