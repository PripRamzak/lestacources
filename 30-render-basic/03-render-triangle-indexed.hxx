#pragma once

#include "02-render-triangle.hxx"

class RenderTriangleIndexed : public RenderTriangle
{
public:
    RenderTriangleIndexed(Canvas& c);
    void DrawTriangle(Pixels&               vertices,
                      std::vector<uint8_t>& indexes,
                      Color                 color);
};
