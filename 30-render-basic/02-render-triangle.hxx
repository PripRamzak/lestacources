#pragma once

#include "01-render-line.hxx"

class RenderTriangle : public RenderLine
{
public:
    RenderTriangle(Canvas& c);
    Pixels GetTrianglePositions(Position vertex_first,
                                Position vertex_second,
                                Position vertex_third);
    void   DrawTriangle(Pixels& vertecises, Color c);
};
