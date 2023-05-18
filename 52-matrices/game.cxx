#include "engine.hxx"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>

#include <glm/matrix.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    if (!engine->load_texture())
        return EXIT_FAILURE;

    std::vector<triangle_2d> texture_triangles;
    texture_triangles.resize(2);
    std::vector<triangle_2d> texture_triangles_centred;
    texture_triangles_centred.resize(2);

    std::ifstream file_vertices;
    std::string   file_name = "./52-matrices/vertices.txt";

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

    float radius = texture_triangles[1].vertices[2].y +
                   (texture_triangles[1].vertices[0].y -
                    texture_triangles[1].vertices[2].y) /
                       2;

    float cur_x = texture_triangles[1].vertices[2].x +
                  (texture_triangles[1].vertices[1].x -
                   texture_triangles[1].vertices[2].x) /
                      2;
    float cur_y = texture_triangles[1].vertices[2].y +
                  (texture_triangles[1].vertices[0].y -
                   texture_triangles[1].vertices[2].y) /
                      2;

    bool  quit = false;
    event event;

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

        float time = engine->get_time();

        float cos   = std::cos(time);
        float sin   = std::sin(time);
        float new_x = radius * cos;
        float new_y = radius * sin;

        // TODO normal rotate
        glm::mat3 mat_move(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, new_x, new_y, 1);
        glm::mat3 mat_move_to_center(
            1.f, 0.f, 0.f, 0.f, 1.f, 0.f, -cur_x, -cur_y, 1);
        glm::mat3 mat_rotate(cos, -1 * sin, 0.f, sin, cos, 0.f, 0.f, 0.f, 1.f);

        for (int i = 0; i < texture_triangles.size(); i++)
            for (int j = 0; j < 3; j++)
            {
                glm::vec3 vec_move(texture_triangles[i].vertices[j].x,
                                   texture_triangles[i].vertices[j].y,
                                   1);
                glm::vec3 vec_rotate(texture_triangles_centred[i].vertices[j].x,
                                     texture_triangles_centred[i].vertices[j].y,
                                     1);

                // TODO multiply matrices
                vec_move = mat_move_to_center * vec_move;
                vec_move = mat_rotate * vec_rotate;
                vec_move = mat_move * vec_move;

                texture_triangles[i].vertices[j].x = vec_move.x;
                texture_triangles[i].vertices[j].y = vec_move.y;
            }

        cur_x = new_x;
        cur_y = new_y;

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
