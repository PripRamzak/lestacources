#include "engine.hxx"

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <thread>

#include <SDL3/SDL.h>

static std::array<std::string_view, 17> event_names = {
    { "Up button pressed",
      "Up button released",
      "Down button pressed",
      "Down button released",
      "Left button pressed",
      "Left button released",
      "Right button pressed",
      "Right button released",
      "Button1 pressed",
      "Button1 released",
      "Button2 pressed",
      "Button2 released",
      "Select button pressed",
      "Select button released",
      "Start button pressed",
      "Start button released",
      "Turn off" }
};

std::ostream& operator<<(std::ostream& out, const Event& event)
{
    int min_value = static_cast<int>(Event::up_pressed);
    int max_value = static_cast<int>(Event::null_event);
    int cur_value = static_cast<int>(event);
    if (cur_value >= min_value && cur_value <= max_value)
    {
        out << event_names[cur_value];
        return out;
    }
    else
        throw std::runtime_error("There is no such event value");
}

struct Bind
{
    SDL_Keycode      key;
    std::string_view name;
    Event            event_pressed;
    Event            event_released;
};

std::array<Bind, 9> key_bindings = {
    { { SDLK_w, "Up", Event::up_pressed, Event::up_released },
      { SDLK_s, "Down", Event::down_pressed, Event::down_released },
      { SDLK_a, "Left", Event::left_pressed, Event::left_released },
      { SDLK_d, "Right", Event::right_pressed, Event::right_released },
      { SDLK_1, "Button 1", Event::button1_pressed, Event::button1_released },
      { SDLK_2, "Button 2", Event::button2_pressed, Event::button2_released },
      { SDLK_e, "Select", Event::select_pressed, Event::select_released },
      { SDLK_SPACE, "Start", Event::start_pressed, Event::start_released },
      { SDLK_ESCAPE, "Close", Event::turn_off, Event::null_event } }
};

bool CheckPressingKey(SDL_Event sdl_event, Event& event)
{
    const auto it = std::find_if(key_bindings.begin(),
                                 key_bindings.end(),
                                 [&](const ::Bind& b)
                                 { return b.key == sdl_event.key.keysym.sym; });
    if (it != key_bindings.end())
    {
        if (sdl_event.type == SDL_EVENT_KEY_DOWN)
            event = it->event_pressed;
        else if (sdl_event.type == SDL_EVENT_KEY_UP)
            event = it->event_released;
        return true;
    }
    return false;
}

class GameEngine final : public Engine
{
    SDL_Window* window;

public:
    GameEngine()
        : window(nullptr)
    {
    }
    bool Initialize() final
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "SDL_INIT error : " << SDL_GetError() << std::endl;
            return false;
        }

        /*window = SDL_CreateWindow("Game", 800, 600, 0);
        if (!window)
        {
            std::cout << "SDL_CreateWindow error: " << SDL_GetError()
                      << std::endl;
            SDL_Quit();
            return false;
        }*/
        return true;
    }
    bool ReadInput(Event& event) final
    {
        SDL_Event sdl_event;

        if (SDL_PollEvent(&sdl_event))
        {
            if (sdl_event.type == SDL_EVENT_QUIT)
            {
                event = Event::turn_off;
                return true;
            }
            else if (sdl_event.type == SDL_EVENT_KEY_DOWN ||
                     sdl_event.type == SDL_EVENT_KEY_UP)
                if (CheckPressingKey(sdl_event, event))
                    return true;
        }
        return false;
    }
    void Uninitialize() final
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

static bool engine_exists = false;

Engine* CreateEngine()
{
    if (engine_exists)
        throw std::runtime_error("Engine already exists");
    Engine* engine = new GameEngine;
    engine_exists  = true;
    return engine;
}

void DestroyEngine(Engine* engine)
{
    if (!engine_exists)
        throw std::runtime_error("Engine doesn't exists");
    if (engine == nullptr)
        throw std::runtime_error("engine is nullptr");
    delete engine;
}

Engine::~Engine() = default;

Game* ReloadGame(Game*       old_game,
                 const char* lib_name,
                 const char* tmp_lib_name,
                 Engine&     engine,
                 void*&      old_handle)
{
    if (old_game)
    {
        SDL_FunctionPointer destroy_game_func_ptr =
            SDL_LoadFunction(old_handle, "DestroyGame");
        if (!destroy_game_func_ptr)
        {
            std::cerr << "Error: can't create function pointer" << tmp_lib_name
                      << "\nError code: " << SDL_GetError() << std::endl;
        }

        using destroy_game_ptr = decltype(&DestroyGame);

        auto destroy_game_func =
            reinterpret_cast<destroy_game_ptr>(destroy_game_func_ptr);

        destroy_game_func(old_game);
        SDL_UnloadObject(old_handle);
    }

    if (std::filesystem::exists(tmp_lib_name))
        if (std::filesystem::remove(tmp_lib_name) == false)
        {
            std::cerr << "Error: can't remove [" << tmp_lib_name << "]"
                      << std::endl;
            return nullptr;
        }

    try
    {
        std::filesystem::copy_file(lib_name, tmp_lib_name);
    }
    catch (std::exception& ex)
    {
        std::cerr << "Error: can't copy [" << lib_name << "] to ["
                  << tmp_lib_name << "]" << std::endl;
        return nullptr;
    }

    void* game_handle = SDL_LoadObject(tmp_lib_name);
    if (!game_handle)
    {
        std::cerr << "Error: failed to load" << tmp_lib_name
                  << "\nError code: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    old_handle = game_handle;

    SDL_FunctionPointer create_game_func_ptr =
        SDL_LoadFunction(game_handle, "CreateGame");
    if (!create_game_func_ptr)
    {
        std::cerr << "Error: can't create function pointer" << tmp_lib_name
                  << "\nError code: " << SDL_GetError() << std::endl;
    }

    using create_game_ptr = decltype(&CreateGame);

    auto create_game_func =
        reinterpret_cast<create_game_ptr>(create_game_func_ptr);

    Game* game = create_game_func(&engine);
    if (!game)
    {
        std::cerr << "Error: function [CreateGame] returned: nullptr"
                  << std::endl;
        return nullptr;
    }

    return game;
}

int main(int argc, char** argv)
{
    std::unique_ptr<Engine, void (*)(Engine*)> engine(CreateEngine(),
                                                      DestroyEngine);

    if (!engine->Initialize())
        return EXIT_FAILURE;

    const char* lib_name     = "./libgame.so";
    const char* tmp_lib_name = "./temp.so";

    void* game_lib_handle = nullptr;

    Game* game =
        ReloadGame(nullptr, lib_name, tmp_lib_name, *engine, game_lib_handle);

    auto loading_lib_time = std::filesystem::last_write_time(lib_name);

    bool  quit = false;
    Event event;

    while (!quit)
    {
        auto current_write_time = std::filesystem::last_write_time(lib_name);

        if (loading_lib_time != current_write_time)
        {
            std::filesystem::file_time_type new_write_time;
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                new_write_time = std::filesystem::last_write_time(lib_name);
                if (new_write_time != current_write_time)
                    current_write_time = new_write_time;
                else
                    break;
            }

            std::cout << "\nReloading game" << std::endl;

            game = ReloadGame(
                game, lib_name, tmp_lib_name, *engine, game_lib_handle);
            if (!game)
            {
                std::cerr << "Failed to reload game. Try to do it again"
                          << std::endl;
                continue;
            }

            loading_lib_time = new_write_time;

            if (loading_lib_time != current_write_time)
                std::cout << "Not good" << std::endl;
        }

        /*if (engine->ReadInput(event))
        {
            std::cout << event << std::endl;
            if (event == Event::turn_off)
            {
                quit = true;
                break;
            }
        }*/

        game->Update();
        game->Render();
    }

    engine->Uninitialize();

    return EXIT_SUCCESS;
}
