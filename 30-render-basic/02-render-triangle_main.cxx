#include "02-render-triangle.hxx"

int main(int argc, char** argv)
{
    const Color black{ 0, 0, 0 };
    const Color white{ 255, 255, 255 };

    size_t width  = 800;
    size_t height = 600;

    Canvas image{ width, height };

    RenderTriangle render(image);
    render.Clear(black);

    int triangles_number_x = 5;
    int triangles_number_y = 5;
    int step_x             = (width - 1) / triangles_number_x;
    int step_y             = (height - 1) / triangles_number_y;

    Pixels triangle_vertices;
    triangle_vertices.reserve(triangles_number_x * triangles_number_y * 6);

    for (int y = 0; y < triangles_number_y; y++)
        for (int x = 0; x < triangles_number_x; x++)
        {
            Position vertex_first{ x * step_x, y * step_y };
            Position vertex_second{ vertex_first.GetX() + step_x,
                                    vertex_first.GetY() };
            Position vertex_third{ vertex_first.GetX() + step_x,
                                   vertex_first.GetY() + step_y };
            Position vertex_forth{ vertex_first.GetX(),
                                   vertex_first.GetY() + step_y };

            for (Position vertex :
                 { vertex_first, vertex_second, vertex_third })
                triangle_vertices.push_back(vertex);

            for (Position vertex : { vertex_first, vertex_third, vertex_forth })
                triangle_vertices.push_back(vertex);
        }

    render.DrawTriangle(triangle_vertices, white);

    std::string filename = "02-triangles.ppm";
    if (image.SaveImage(filename))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
