#pragma once

#include "04-render-triangle-interpolated.hxx"

class RenderProgram : public RenderTriangleInterpolated
{
    GFXProgram* program = nullptr;

public:
    RenderProgram(Canvas& c);
    void SetProgram(GFXProgram* program);
    void DrawTriangles(std::vector<Vertex>&  vertices,
                       std::vector<uint8_t>& indexes) override;
};

struct Program : public GFXProgram
{
    double mouse_x;
    double mouse_y;
    double radius;
    void   SetUniforms(Uniform& uniform) override;
    Vertex VertexShader(Vertex& vertex) override;
    Color  FragmentShader(Vertex& vertex) override;
};
