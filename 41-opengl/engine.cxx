#include "engine.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <thread>

#include <SDL3/SDL.h>

#include "glad/glad.h"

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

std::ostream& operator<<(std::ostream& out, const event& event)
{
    int min_value = static_cast<int>(event::up_pressed);
    int max_value = static_cast<int>(event::null_event);
    int cur_value = static_cast<int>(event);
    if (cur_value >= min_value && cur_value <= max_value)
    {
        out << event_names[cur_value];
        return out;
    }
    else
        throw std::runtime_error("There is no such event value");
}

struct bind
{
    SDL_Keycode      key;
    std::string_view name;
    event            event_pressed;
    event            event_released;
};

std::array<bind, 9> key_bindings = {
    { { SDLK_w, "Up", event::up_pressed, event::up_released },
      { SDLK_s, "Down", event::down_pressed, event::down_released },
      { SDLK_a, "Left", event::left_pressed, event::left_released },
      { SDLK_d, "Right", event::right_pressed, event::right_released },
      { SDLK_1, "Button 1", event::button1_pressed, event::button1_released },
      { SDLK_2, "Button 2", event::button2_pressed, event::button2_released },
      { SDLK_e, "Select", event::select_pressed, event::select_released },
      { SDLK_SPACE, "Start", event::start_pressed, event::start_released },
      { SDLK_ESCAPE, "Close", event::turn_off, event::null_event } }
};

bool CheckPressingKey(SDL_Event sdl_event, event& event)
{
    const auto it = std::find_if(key_bindings.begin(),
                                 key_bindings.end(),
                                 [&](const ::bind& b)
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

void gl_check()
{
    const int error = static_cast<int>(glGetError());
    if (error != GL_NO_ERROR)
    {
        switch (error)
        {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM error" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE error" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPEARATION error" << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPEARATION error"
                          << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY error" << std::endl;
                break;
        }
        assert(false);
    }
}

class game_engine final : public engine
{
    SDL_Window*   window;
    SDL_GLContext context;
    GLuint        program;

public:
    game_engine()
        : window(nullptr)
        , context(nullptr)
        , program(0)
    {
    }
    bool initialize() final
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cerr << "Init error : " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Game", 800, 600, SDL_WINDOW_OPENGL);
        if (!window)
        {
            std::cerr << "CreateWindow error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

        // create context based on os
        std::string_view platform{ SDL_GetPlatform() };
        using namespace std::string_view_literals;
        auto       os_list = { "Windows"sv, "Mac OS X"sv };
        const auto it = std::find(os_list.begin(), os_list.end(), platform);
        int        gl_major_version;
        int        gl_minor_version;
        int        gl_profile_mask;
        if (it != os_list.end())
        {
            gl_major_version = 4;
            gl_minor_version = (platform == "Mac OS X"sv) ? 1 : 3;
            gl_profile_mask  = SDL_GL_CONTEXT_PROFILE_CORE;
        }
        else
        {
            gl_major_version = 3;
            gl_minor_version = 2;
            gl_profile_mask  = SDL_GL_CONTEXT_PROFILE_ES;
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_profile_mask);

        context = SDL_GL_CreateContext(window);
        if (context == nullptr)
        {
            std::cerr << "CreateContext error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                                &gl_major_version))
        {
            std::cerr << "GL_GetAttribute error: " << SDL_GetError()
                      << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (platform == "Linux"sv &&
            (gl_major_version != 3 || gl_minor_version != 2 ||
             gl_profile_mask != SDL_GL_CONTEXT_PROFILE_ES))
        {
            std::cerr << "Current OpengGL version: " << gl_major_version << '.'
                      << gl_minor_version << " ES" << std::endl;
            std::cerr << "Need OpenGL version: 3.2 ES" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        else if (platform == "Windows"sv && gl_major_version < 4 &&
                 gl_minor_version < 3)
        {
            std::cerr << "Current OpengGL version: " << gl_major_version << '.'
                      << gl_minor_version << std::endl;
            std::cerr << "At least need OpenGL version: 4.3" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        else if (platform == "Mac OS X"sv && gl_major_version < 4 &&
                 gl_minor_version < 1)
        {
            std::cerr << "Current OpengGL version: " << gl_major_version << '.'
                      << gl_minor_version << std::endl;
            std::cerr << "At least need OpenGL version: 4.1" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        std::cout << "OpenGL " << gl_major_version << '.' << gl_minor_version
                  << std::endl;

        // load OpenGL functions
        auto gl_function_loader = [](const char* function_name)
        {
            SDL_FunctionPointer function_ptr =
                SDL_GL_GetProcAddress(function_name);
            return reinterpret_cast<void*>(function_ptr);
        };

        if (!gladLoadGLES2Loader(gl_function_loader))
        {
            std::cerr << "LoadGlFunctions error" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        // enable debug
        /*if (platform != "Mac OS X")
        {
            glEnable(GL_DEBUG_OUTPUT);
            gl_check();
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            gl_check();
            glDebugMessageControl(
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            gl_check();
        }*/

        // create vertex shader
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        // TODO
        const char* vertex_shader_src = R"(
                                    #version 300 es
                                    in vec3 a_position;
                                    out vec4 v_position;

                                    void main()
                                    {
                                        v_position = vec4(a_position, 1.0);
                                        gl_Position = v_position;
                                    }
                                    )";
        glShaderSource(vertex_shader, 1, &vertex_shader_src, nullptr);
        gl_check();
        glCompileShader(vertex_shader);
        gl_check();

        GLint compiled_status = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled_status);
        gl_check();
        if (compiled_status == 0)
        {
            GLint info_length = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &info_length);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_length));
            glGetShaderInfoLog(
                vertex_shader, info_length, nullptr, info_log.data());
            gl_check();
            glDeleteShader(vertex_shader);
            gl_check();

            std::cerr << "Error while compiling vertex shader\nSourceCode:\n"
                      << vertex_shader_src << std::endl
                      << info_log.data();

            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        // create fragment shader
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        gl_check();
        const char* fragment_shader_src = R"(
                      #version 300 es
                      precision mediump float;

                      in vec4 v_position;
                      uniform float time;
                      out vec4 frag_color;

                      void main()
                      {
                          float blue_color = abs((v_position.x - v_position.y) / 2. * sin(time));
                          frag_color = vec4(abs(sin(time) * v_position.x), abs(sin(time) * v_position.y), blue_color, 1.0);
                      }
                      )";
        glShaderSource(fragment_shader, 1, &fragment_shader_src, nullptr);
        gl_check();
        glCompileShader(fragment_shader);
        gl_check();

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
        gl_check();
        if (compiled_status == 0)
        {
            GLint info_length = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_length);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_length));
            glGetShaderInfoLog(
                fragment_shader, info_length, nullptr, info_log.data());
            gl_check();

            std::cerr << "Error while compiling fragment shader\nSourceCode:\n"
                      << fragment_shader_src << std::endl
                      << info_log.data();

            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        // Create program and attach shaders

        program = glCreateProgram();
        gl_check();
        if (program == 0)
        {
            std::cerr << "Create program error" << std::endl;

            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        glAttachShader(program, vertex_shader);
        gl_check();
        glAttachShader(program, fragment_shader);
        gl_check();

        glBindAttribLocation(program, 0, "a_program");
        gl_check();

        glLinkProgram(program);
        gl_check();

        GLint linked_status = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked_status);
        gl_check();
        if (linked_status == 0)
        {
            GLint info_lenght = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_lenght);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_lenght));
            glGetProgramInfoLog(program, info_lenght, nullptr, info_log.data());
            gl_check();

            std::cerr << "Error while linkind program: " << info_log.data();

            glDeleteProgram(program);
            gl_check();
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        glUseProgram(program);
        gl_check();

        glEnable(GL_DEPTH_TEST);
        gl_check();

        return true;
    }
    bool read_input(event& event) final
    {
        SDL_Event sdl_event;

        if (SDL_PollEvent(&sdl_event))
        {
            if (sdl_event.type == SDL_EVENT_QUIT)
            {
                event = event::turn_off;
                return true;
            }
            else if (sdl_event.type == SDL_EVENT_KEY_DOWN ||
                     sdl_event.type == SDL_EVENT_KEY_UP)
                if (CheckPressingKey(sdl_event, event))
                    return true;
        }
        return false;
    }
    void clear() final
    {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        gl_check();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_check();
    }
    void set_uniforms(const char* name, float value) final
    {
        GLint uniform_location = glGetUniformLocation(program, name);
        glUniform1f(uniform_location, value);
    }
    void generate_triangles(std::vector<triangle>& triangles) final
    {
        constexpr int triangles_number_x = 10;
        constexpr int triangles_number_y = 10;
        const float   step_x             = 2. / triangles_number_x;
        const float   step_y             = 2. / triangles_number_x;

        std::vector<vertex> vertices;
        vertices.reserve((triangles_number_x + 1) * (triangles_number_y + 1));

        for (int y = 0; y <= triangles_number_y; y++)
            for (int x = 0; x <= triangles_number_x; x++)
            {
                vertex vertex{ -1 + step_x * x, 1 - step_y * y, 0.f };
                vertices.push_back(vertex);
            }

        std::vector<uint8_t> indexes;

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
                    index_second =
                        (y + 1) * triangles_number_y + (y + 1) + x + 1;
                    index_third = index_second - 1;
                }

                for (uint8_t index : { index_first, index_second, index_third })
                    indexes.push_back(index);
            }

        for (size_t i = 0; i < indexes.size(); i += 3)
        {
            uint8_t index_first  = indexes.at(i);
            uint8_t index_second = indexes.at(i + 1);
            uint8_t index_third  = indexes.at(i + 2);

            triangles.push_back({ vertices.at(index_first),
                                  vertices.at(index_second),
                                  vertices.at(index_third) });
        }
    }
    void draw_triangles(const triangle& triangle) final
    {
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), &triangle.vertices[0]);
        gl_check();
        glEnableVertexAttribArray(0);
        gl_check();

        glValidateProgram(program);
        gl_check();
        GLint validate_status = 0;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_status);
        gl_check();
        if (validate_status == GL_FALSE)
        {
            GLint info_lenght = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_lenght);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_lenght));
            glGetProgramInfoLog(program, info_lenght, nullptr, info_log.data());
            gl_check();

            std::cerr << "Incorrect validate status: " << info_log.data()
                      << std::endl;
            uninitialize();

            throw std::runtime_error("Error");
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_check();
    }
    bool swap_bufers() final
    {
        if (SDL_GL_SwapWindow(window) != 0)
        {
            std::cerr << "Swap Window error:" << SDL_GetError() << std::endl;
            return false;
        }
        clear();

        return true;
    }
    void uninitialize() final
    {
        glDeleteProgram(program);
        gl_check();
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

static bool engine_exists = false;

engine* create_engine()
{
    if (engine_exists)
        throw std::runtime_error("Engine already exists");
    engine* engine = new game_engine;
    engine_exists  = true;
    return engine;
}

void destroy_engine(engine* engine)
{
    if (!engine_exists)
        throw std::runtime_error("Engine doesn't exists");
    if (engine == nullptr)
        throw std::runtime_error("engine is nullptr");
    delete engine;
}

engine::~engine() = default;
