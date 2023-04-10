#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_version.h>

SDL_Texture* RenderText(const std::string& message,
                        const std::string& fontFile,
                        SDL_Color          color,
                        int                fontSize,
                        SDL_Renderer*      renderer)
{
    TTF_Font* font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (font == nullptr)
    {
        std::cout << "TTF_OpenFont error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
    if (surf == nullptr)
    {
        TTF_CloseFont(font);
        std::cout << "TTF_OpenFont error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (texture == nullptr)
    {
        std::cout << "CreateTexture error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_DestroySurface(surf);
    TTF_CloseFont(font);
    return texture;
}

void RenderTexture(SDL_Texture*  tex,
                   SDL_Renderer* ren,
                   int           x,
                   int           y,
                   SDL_FRect*    clip = nullptr)
{
    SDL_FRect dst;
    dst.x = x;
    dst.y = y;
    if (clip != nullptr)
    {
        dst.w = clip->w;
        dst.h = clip->h;
    }
    else
    {
        int w, h;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);
        dst.w = float(w);
        dst.h = float(h);
    }
    SDL_RenderTexture(ren, tex, clip, &dst);
}

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL_INIT error : " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    if (TTF_Init() != 0)
    {
        std::cout << "TTF_INIT error : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Window* window =
        SDL_CreateWindow("SDL Version", 800, 600, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        std::cout << "CreateWindow error : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, nullptr, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
    {
        std::cout << "CreateRenderer error : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    char*       base_path = SDL_GetBasePath();
    std::string font_path = base_path;
    SDL_free(base_path);
    size_t pos = font_path.rfind("build");
    font_path  = font_path.substr(0, pos) + "res/fonts/joystix.ttf";

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled)
    SDL_GetVersion(&linked);

    std::to_string(compiled.major);

    std::string message_compiled =
        "compiled " + std::to_string(compiled.major) + "." +
        std::to_string(compiled.minor) + "." + std::to_string(compiled.patch);
    std::string message_linked = "linked " + std::to_string(linked.major) +
                                 "." + std::to_string(linked.minor) + "." +
                                 std::to_string(linked.patch);

    SDL_Color    color = { 255, 255, 255, 255 };
    SDL_Texture* text_compiled =
        RenderText(message_compiled, font_path, color, 64, renderer);
    if (text_compiled == nullptr)
    {
        SDL_DestroyTexture(text_compiled);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_Texture* text_linked =
        RenderText(message_linked, font_path, color, 64, renderer);
    if (text_compiled == nullptr)
    {
        SDL_DestroyTexture(text_linked);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    int text_compiled_height, text_compiled_width, text_linked_width,
        text_linked_height;
    SDL_QueryTexture(text_compiled,
                     nullptr,
                     nullptr,
                     &text_compiled_width,
                     &text_compiled_height);
    SDL_QueryTexture(text_compiled,
                     nullptr,
                     nullptr,
                     &text_linked_width,
                     &text_linked_height);

    int posX_compiled = 400 - text_compiled_width / 2;
    int posY_compiled = 300 - text_compiled_height;
    int posX_linked   = 400 - text_linked_width / 2;
    int posY_linked   = 300 + text_linked_height;

    SDL_Event event;
    bool      quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                quit = true;
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
        }
        SDL_RenderClear(renderer);
        RenderTexture(text_compiled, renderer, posX_compiled, posY_compiled);
        RenderTexture(text_linked, renderer, posX_linked, posY_linked);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(text_compiled);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
}
