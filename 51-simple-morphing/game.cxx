#include "engine.hxx"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    using namespace std::chrono;

    high_resolution_clock::time_point time_start = high_resolution_clock::now();

    bool  quit = false;
    event event;

    std::vector<triangle> circle_triangles;
    std::vector<triangle> star_triangles;
    engine->generate_circle(circle_triangles);

    std::ifstream file_vertices;
    std::string   file_name = "./51-simple-morphing/vertices.txt";

    file_vertices.exceptions(std::ifstream::failbit);
    try
    {
        file_vertices.open(file_name);
        while (!file_vertices.eof())
        {
            triangle triangle;
            file_vertices >> triangle;
            star_triangles.push_back(triangle);
        }
        file_vertices.close();
    }
    catch (std::ifstream::failure)
    {
        std::cerr << "Exception opening/reading/closing file1" << std::endl;
        return EXIT_FAILURE;
    }
    while (!quit)
    {

        if (engine->read_input(event))
        {
            std::cout << event << std::endl;
            if (event == event::turn_off)
            {
                quit = true;
                break;
            }
        }

        float time_value = (time_start - high_resolution_clock::now()).count() /
                           1'000'000'000.f;
        engine->set_uniforms("time", 1);

        float alpha = sin(time_value) * 0.5 + 0.5;

        for (int i = 0; i < 10; i++)
        {
            triangle t =
                transform(circle_triangles[i], star_triangles[i], alpha);
            engine->draw_triangles(t);
        }

        if (!engine->swap_buffers())
        {
            engine->uninitialize();
            return EXIT_FAILURE;
        }
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
