#include "02-render-triangle.hxx"

#include <utility>

RenderTriangle::RenderTriangle(Canvas& c)
    : RenderLine(c)
{
}

Pixels RenderTriangle::GetTrianglePositions(Position vertex_first,
                                            Position vertex_second,
                                            Position vertex_third)
{
    Pixels line, triangle_pixels;

    for (auto [start, end] : { std::pair{ vertex_first, vertex_second },
                               std::pair{ vertex_second, vertex_third },
                               std::pair{ vertex_third, vertex_first } })
    {
        line = RenderLine::GetPositions(start, end);
        triangle_pixels.insert(triangle_pixels.end(), line.begin(), line.end());
    }

    return triangle_pixels;
}
void RenderTriangle::DrawTriangle(Pixels& vertices, Color c)
{
    Position vertex_first, vertex_second, vertex_third;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        vertex_first  = vertices.at(i);
        vertex_second = vertices.at(i + 1);
        vertex_third  = vertices.at(i + 2);

        for (auto triangle_pixel :
             GetTrianglePositions(vertex_first, vertex_second, vertex_third))
            SetPixel(triangle_pixel, c);
    }
}
