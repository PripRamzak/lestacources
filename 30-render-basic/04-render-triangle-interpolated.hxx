#pragma once

#include "03-render-triangle-indexed.hxx"

struct Position3D
{
    double x;
    double y;
    double z;
};

struct Vertex
{
    Position3D position;
    Color      color;
};

class RenderTriangleInterpolated : public RenderTriangleIndexed
{
protected:
    void RasterizeHorizontalLine(Vertex&              left,
                                 Vertex&              right,
                                 std::vector<Vertex>& pixels);
    void RasterizeHorizontalTriangle(Vertex&              vertex_left,
                                     Vertex&              vertex_right,
                                     Vertex&              vertex_single,
                                     std::vector<Vertex>& pixels);
    void RasterizeTriangle(Vertex&              vertex_first,
                           Vertex&              vertex_second,
                           Vertex&              vertex_third,
                           std::vector<Vertex>& pixels);

public:
    RenderTriangleInterpolated(Canvas& c);
    virtual void DrawTriangles(std::vector<Vertex>&  vertices,
                               std::vector<uint8_t>& indexes);
};

struct Uniform
{
    double u0;
    double u1;
    double u2;
};

struct GFXProgram
{
    virtual void   SetUniforms(Uniform& uniform)  = 0;
    virtual Vertex VertexShader(Vertex& vertex)   = 0;
    virtual Color  FragmentShader(Vertex& vertex) = 0;
    virtual ~GFXProgram();
};
