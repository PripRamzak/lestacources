#include "05-render-program.hxx"

#include <cstdlib>
#include <iostream>

#include <SDL3/SDL.h>

int main(int argc, char** argv)
{
    constexpr Color black{ 0, 0, 0 };

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Init error : " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    constexpr size_t width  = 800;
    constexpr size_t height = 600;

    SDL_Window* window =
        SDL_CreateWindow("GFX Program", 800, 600, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        std::cerr << "CreateWindow error : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "CreateRenderer error : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    Canvas image(width, height);

    Program       program;
    RenderProgram render(image);
    render.SetProgram(&program);
    render.Clear(black);

    constexpr int    triangles_number_x = 12;
    constexpr int    triangles_number_y = 12;
    constexpr double step_x =
        static_cast<double>((width - 1)) / triangles_number_x;
    constexpr double step_y =
        static_cast<double>((height - 1)) / triangles_number_y;

    std::vector<Vertex> vertices;
    vertices.reserve((triangles_number_x + 1) * (triangles_number_y + 1));

    for (int y = 0; y <= triangles_number_y; y++)
        for (int x = 0; x <= triangles_number_x; x++)
        {
            Vertex vertex{ x * step_x, y * step_y, 0, Color::GenerateRandom() };
            vertices.push_back(vertex);
        }

    std::vector<uint8_t> indexes;
    indexes.reserve((triangles_number_x + 1) * (triangles_number_y + 1) * 6);

    for (int y = 0; y < triangles_number_y; y++)
        for (int x = 0; x < triangles_number_x; x++)
        {
            uint8_t index_first, index_second, index_third;

            if (y % 2 == 0)
            {
                index_first  = y * triangles_number_y + y + x;
                index_second = index_first + 1;
                if (x % 2 == 0)
                    index_third = (y + 1) * triangles_number_y + y + x + 1;
                else
                    index_third = (y + 1) * triangles_number_y + y + x + 2;
            }
            else
            {
                if (x % 2 == 0)
                    index_first = y * triangles_number_y + y + x;
                else
                    index_first = y * triangles_number_y + y + x + 1;
                index_second = (y + 1) * triangles_number_y + (y + 1) + x + 1;
                index_third  = index_second - 1;
            }

            for (uint8_t index : { index_first, index_second, index_third })
                indexes.push_back(index);
        }

    void* pixels = image.GetPixels().data();
    int   pitch  = sizeof(Color) * width;

    double mouse_x = 0;
    double mouse_y = 0;
    double radius  = 40;

    SDL_Event event;
    bool      quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                quit = true;
                break;
            }
            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
            }
            if (event.type == SDL_EVENT_MOUSE_WHEEL)
                radius += event.wheel.y;
        }

        Uniform uniforms{ mouse_x, mouse_y, radius };
        program.SetUniforms(uniforms);

        render.Clear(black);
        render.DrawTriangles(vertices, indexes);

        SDL_Surface* surface = SDL_CreateSurfaceFrom(
            pixels, width, height, pitch, SDL_PIXELFORMAT_RGB24);
        if (surface == nullptr)
        {
            std::cerr << "CreateSurface error : " << SDL_GetError()
                      << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return EXIT_FAILURE;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == nullptr)
        {
            std::cerr << "CreateTexture error : " << SDL_GetError()
                      << std::endl;
            SDL_DestroySurface(surface);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return EXIT_FAILURE;
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_DestroySurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
