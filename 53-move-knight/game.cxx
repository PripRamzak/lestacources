#include "engine.hxx"

#include <chrono>
#include <cstdlib>
#include <fstream>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    if (!engine->load_texture())
        return EXIT_FAILURE;

    int direction = 0;

    std::vector<triangle_2d> texture_triangles;
    texture_triangles.resize(2);
    std::vector<triangle_2d> texture_triangles_centred;
    texture_triangles_centred.resize(2);

    std::ifstream file_vertices;
    std::string   file_name = "./53-move-knight/vertices.txt";

    file_vertices.exceptions(std::ifstream::failbit);
    try
    {
        file_vertices.open(file_name);
        file_vertices >> texture_triangles[0];
        file_vertices >> texture_triangles[1];
        file_vertices >> texture_triangles_centred[0];
        file_vertices >> texture_triangles_centred[1];
        file_vertices.close();
    }
    catch (std::ifstream::failure)
    {
        std::cerr << "Exception opening/reading/closing file1" << std::endl;
        return EXIT_FAILURE;
    }

    bool  quit = false;
    event event;

    while (!quit)
    {

        if (engine->read_input(event))
        {
            std::cout << event << std::endl;
            if (event == event::up_pressed)
                engine->move_texture(texture_triangles, 0.f, 0.03f, direction);
            else if (event == event::down_pressed)
                engine->move_texture(texture_triangles, 0.f, -0.03f, direction);
            else if (event == event::left_pressed)
                engine->move_texture(texture_triangles, -0.03f, 0.f, direction);
            else if (event == event::right_pressed)
                engine->move_texture(texture_triangles, 0.03f, 0.f, direction);
            if (event == event::turn_off)
            {
                quit = true;
                break;
            }
        }

        engine->draw_triangles_2d(texture_triangles[0]);
        engine->draw_triangles_2d(texture_triangles[1]);

        if (!engine->swap_buffers())
        {
            engine->uninitialize();
            return EXIT_FAILURE;
        }
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
