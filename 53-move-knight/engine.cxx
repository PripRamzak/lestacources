#include "engine.hxx"
#include "shader_program.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <stdexcept>

#include <SDL3/SDL.h>

#include "glad/glad.h"

#include <glm/matrix.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool check_pressing_key(SDL_Event sdl_event, event& event);

void gl_check();

class game_engine final : public engine
{
    SDL_Window*    window;
    SDL_GLContext  context;
    shader_program program;

public:
    game_engine()
        : window(nullptr)
        , context(nullptr)
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
            std::cerr << "Current OpenGL version: " << gl_major_version << '.'
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
            std::cerr << "Current OpenGL version: " << gl_major_version << '.'
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
            std::cerr << "Current OpenGL version: " << gl_major_version << '.'
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

        if (!program.create_shader("./53-move-knight/vertex_shader.glsl",
                                   shader_type::vertex))
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        if (!program.create_shader("./53-move-knight/fragment_shader.glsl",
                                   shader_type::fragment))
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (!program.create_program())
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

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
                if (check_pressing_key(sdl_event, event))
                    return true;
        }
        return false;
    }

    float get_time() final
    {
        std::uint32_t ms = SDL_GetTicks();
        return ms * 0.001f;
    }

    void clear() final
    {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        gl_check();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_check();
    }

    void set_uniform(const char* name, float value) final
    {
        GLint uniform_location =
            glGetUniformLocation(program.get_program(), name);
        glUniform1f(uniform_location, value);
    }

    bool load_texture() final
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* image = stbi_load(
            "./53-move-knight/knight.png", &width, &height, &channels, 0);
        if (image == nullptr)
        {
            std::cerr << "Failed to load image" << std::endl;
            return false;
        }

        glActiveTexture(GL_TEXTURE0);
        gl_check();
        GLuint texture = 0;
        glGenTextures(1, &texture);
        gl_check();
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_check();
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     width,
                     height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     image);
        gl_check();
        glGenerateMipmap(GL_TEXTURE_2D);
        gl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl_check();

        GLint location = glGetUniformLocation(program.get_program(), "texture");
        gl_check();
        assert(-1 != location);
        glUniform1i(location, 0);
        gl_check();

        stbi_image_free(image);
        return true;
    }

    void move_texture(std::vector<triangle_2d>& texture_triangles,
                      float                     dx,
                      float                     dy,
                      int&                      direction) final
    {
        bool new_direction = false;
        if (dx < 0 && direction != 1 || dx > 0 && direction != 0)
        {
            new_direction = true;
            direction     = direction == 0 ? 1 : 0;
        }

        glm::mat3 mat_move(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, dx, dy, 1);
        std::vector<triangle_2d> buffer_triangles = texture_triangles;

        for (int i = 0; i < texture_triangles.size(); i++)
            for (int j = 0; j < 3; j++)
            {
                glm::vec3 vec_move(texture_triangles[i].vertices[j].x,
                                   texture_triangles[i].vertices[j].y,
                                   1);

                if (new_direction)
                {
                    float mirror_dx = texture_triangles[0].vertices[1].x -
                                      texture_triangles[0].vertices[0].x;
                    mirror_dx = buffer_triangles[i].vertices[j].u == 0.f
                                    ? mirror_dx
                                    : -mirror_dx;

                    buffer_triangles[i].vertices[j].u =
                        buffer_triangles[i].vertices[j].u == 0.f ? 1.f : 0.f;

                    glm::mat3 mat_mirror(
                        1.f, 0.f, 0.f, 0.f, 1.f, 0.f, mirror_dx, 0, 1);
                }
                vec_move = mat_move * vec_move;

                if (std::abs(vec_move.y) > 1.f || std::abs(vec_move.x) > 1.f)
                    return;

                buffer_triangles[i].vertices[j].x = vec_move.x;
                buffer_triangles[i].vertices[j].y = vec_move.y;
            }

        for (int i = 0; i < texture_triangles.size(); i++)
            texture_triangles[i] = buffer_triangles[i];
    }

    void generate_triangles(std::vector<triangle>& triangles) final
    {
        constexpr int triangles_number_x = 10;
        constexpr int triangles_number_y = 10;
        const float   step_x             = 2. / triangles_number_x;
        const float   step_y             = 2. / triangles_number_x;

        std::vector<struct vertex> vertices;
        vertices.reserve((triangles_number_x + 1) * (triangles_number_y + 1));

        for (int y = 0; y <= triangles_number_y; y++)
            for (int x = 0; x <= triangles_number_x; x++)
            {
                struct vertex vertex
                {
                    - 1 + step_x *x, 1 - step_y *y, 0.f
                };
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

    void draw_triangles_2d(const triangle_2d& triangle) final
    {
        glVertexAttribPointer(0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
                              &triangle.vertices[0].x);
        gl_check();
        glEnableVertexAttribArray(0);
        gl_check();

        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
                              &triangle.vertices[0].u);
        gl_check();
        glEnableVertexAttribArray(1);
        gl_check();

        glValidateProgram(program.get_program());
        gl_check();
        GLint validate_status = 0;
        glGetProgramiv(
            program.get_program(), GL_VALIDATE_STATUS, &validate_status);
        gl_check();
        if (validate_status == GL_FALSE)
        {
            GLint info_length = 0;
            glGetProgramiv(
                program.get_program(), GL_INFO_LOG_LENGTH, &info_length);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_length));
            glGetProgramInfoLog(
                program.get_program(), info_length, nullptr, info_log.data());
            gl_check();

            std::cerr << "Incorrect validate status: " << info_log.data()
                      << std::endl;
            uninitialize();

            throw std::runtime_error("Error");
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_check();
    }

    void draw_triangles(const triangle& triangle) final
    {
        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &triangle.vertices[0]);
        gl_check();
        glEnableVertexAttribArray(0);
        gl_check();

        glValidateProgram(program.get_program());
        gl_check();
        GLint validate_status = 0;
        glGetProgramiv(
            program.get_program(), GL_VALIDATE_STATUS, &validate_status);
        gl_check();
        if (validate_status == GL_FALSE)
        {
            GLint info_length = 0;
            glGetProgramiv(
                program.get_program(), GL_INFO_LOG_LENGTH, &info_length);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_length));
            glGetProgramInfoLog(
                program.get_program(), info_length, nullptr, info_log.data());
            gl_check();

            std::cerr << "Incorrect validate status: " << info_log.data()
                      << std::endl;
            uninitialize();

            throw std::runtime_error("Error");
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_check();
    }

    void generate_circle(std::vector<triangle>& circle_triangles) final
    {
        float x             = 0.f;
        float y             = 0.f;
        float radius        = 0.8f;
        int   triangles_num = 10;
        float pi            = std::numbers::pi_v<float>;
        float angle         = 2 * pi / triangles_num;
        float new_x         = x;
        float new_y         = y + radius;

        circle_triangles.reserve(static_cast<size_t>(triangles_num));
        for (int i = 1; i <= triangles_num; i++)
        {
            float prev_x = new_x;
            float prev_y = new_y;
            new_x        = radius * sin(i * angle);
            new_y        = radius * cos(i * angle);
            circle_triangles.push_back({ { x, y, 0.0 },
                                         { prev_x, prev_y, 0.0 },
                                         { new_x, new_y, 0.0 } });
        }
    }

    bool swap_buffers() final
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
        program.delete_progam();
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
