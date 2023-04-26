#include "03-render-triangle-indexed.hxx"

RenderTriangleIndexed::RenderTriangleIndexed(Canvas& c)
    : RenderTriangle(c)
{
}

void RenderTriangleIndexed::DrawTriangleIndexed(Pixels&               vertices,
                                                std::vector<uint8_t>& indexes,
                                                Color                 color)

{
    for (size_t i = 0; i < indexes.size(); i += 3)
    {
        uint8_t index_first  = indexes.at(i);
        uint8_t index_second = indexes.at(i + 1);
        uint8_t index_third  = indexes.at(i + 2);

        Position vertex_first  = vertices.at(index_first);
        Position vertex_second = vertices.at(index_second);
        Position vertex_third  = vertices.at(index_third);

        for (auto pixel :
             GetTrianglePositions(vertex_first, vertex_second, vertex_third))
            SetPixel(pixel, color);
    }
}
