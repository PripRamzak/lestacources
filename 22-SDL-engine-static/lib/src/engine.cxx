#include <engine.hxx>

#include <algorithm>
#include <array>
#include <stdexcept>

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
    int max_value = static_cast<int>(Event::turn_off);
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

        window = SDL_CreateWindow("Game", 800, 600, 0);
        if (!window)
        {
            std::cout << "SDL_CreateWindow error: " << SDL_GetError()
                      << std::endl;
            SDL_Quit();
            return false;
        }
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
