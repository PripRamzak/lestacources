#include "engine.hxx"

#include <chrono>
#include <cmath>

int main(int argc, char** argv)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    std::chrono::high_resolution_clock::time_point time_start =
        std::chrono::high_resolution_clock::now();

    bool  quit = false;
    event event;

    std::vector<triangle> triangles;
    engine->generate_triangles(triangles);

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

        for (auto triangle : triangles)
        {
            float time_value =
                (time_start - std::chrono::high_resolution_clock::now())
                    .count() /
                1'000'000'000.f;
            engine->set_uniforms("time", time_value);

            engine->draw_triangles(triangle);
        }

        if (!engine->swap_bufers())
        {
            engine->uninitialize();
            return EXIT_FAILURE;
        }
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
