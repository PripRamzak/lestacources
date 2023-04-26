#include "03-render-triangle-indexed.hxx"

int main(int argc, char** argv)
{
    const Color black{ 0, 0, 0 };
    const Color white{ 255, 255, 255 };

    size_t width  = 800;
    size_t height = 600;

    Canvas image{ width, height };

    RenderTriangleIndexed render(image);
    render.Clear(black);

    int triangles_number_x = 5;
    int triangles_number_y = 5;
    int step_x             = (width - 1) / triangles_number_x;
    int step_y             = (height - 1) / triangles_number_y;

    Pixels vertices;
    vertices.reserve((triangles_number_x + 1) * (triangles_number_y + 1));

    for (int y = 0; y <= triangles_number_y; y++)
        for (int x = 0; x <= triangles_number_x; x++)
        {
            Position vertex{ x * step_x, y * step_y };
            vertices.push_back(vertex);
        }

    std::vector<uint8_t> indexes;

    for (int y = 0; y < triangles_number_y; y++)
        for (int x = 0; x < triangles_number_x; x++)
        {
            uint8_t index_first  = y * (triangles_number_y + 1) + x;
            uint8_t index_second = index_first + 1;
            uint8_t index_third  = (y + 1) * (triangles_number_y + 1) + x + 1;
            uint8_t index_forth  = index_third - 1;

            for (uint8_t index : { index_first, index_second, index_third })
                indexes.push_back(index);

            for (uint8_t index : { index_first, index_third, index_forth })
                indexes.push_back(index);
        }

    render.DrawTriangleIndexed(vertices, indexes, white);

    std::string filename = "03-triangles-indexed.ppm";
    if (image.SaveImage(filename))
        return EXIT_FAILURE;

    /*render.Clear(black);
    vertices.clear();
    indexes.clear();

    for (int y = 0; y <= triangles_number_y / 2; y++)
        for (int x = y; x <= triangles_number_x - y; x++)
        {
            Position vertex{ x * step_x, y * step_y };
            vertices.push_back(vertex);
        }

    for (int y = 0; y < triangles_number_y / 2; y++)
        for (int x = y; x < triangles_number_x - y; x++)
        {
            uint8_t index_first  = y * (triangles_number_y + 1) + x - y;
            uint8_t index_second = index_first + 1;
            uint8_t index_third  = (y + 1) * (triangles_number_y + 1) + x - y;
            uint8_t index_forth  = index_third - 1;

            if (x < triangles_number_x / 2)
            {
                for (uint8_t index : { index_first, index_second, index_third })
                    indexes.push_back(index);

                if (x == y)
                    continue;
                for (uint8_t index : { index_first, index_third, index_forth })
                    indexes.push_back(index);
            }
            else
            {
                for (uint8_t index : { index_first, index_second, index_forth })
                    indexes.push_back(index);

                if (x + 1 == triangles_number_x - y)
                    continue;
                for (uint8_t index : { index_second, index_third, index_forth })
                    indexes.push_back(index);
            }
        }

    render.DrawTriangleIndexed(vertices, indexes, white);

    filename = "03-big-triangle-from-small.ppm";
    if (image.SaveImage(filename))
        return EXIT_FAILURE;*/

    return EXIT_SUCCESS;
}
