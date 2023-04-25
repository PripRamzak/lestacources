#pragma once

#include "00-canvas.hxx"

class RenderLine : public Render
{
    Canvas& canvas;

public:
    RenderLine(Canvas& c);
    void   Clear(Color c) override;
    void   SetPixel(Position p, Color c) override;
    Pixels GetPositions(Position start, Position end) override;
    void   DrawBresenhamLine(Position start, Position end, Color c);
};
