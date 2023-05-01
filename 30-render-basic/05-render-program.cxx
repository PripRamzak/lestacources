#include "05-render-program.hxx"

#include <cmath>

RenderProgram::RenderProgram(Canvas& c)
    : RenderTriangleInterpolated(c)
{
}

void RenderProgram::SetProgram(GFXProgram* program)
{
    RenderProgram::program = program;
}

void RenderProgram::DrawTriangles(std::vector<Vertex>&  vertices,
                                  std::vector<uint8_t>& indexes)
{
    for (size_t i = 0; i < indexes.size(); i += 3)
    {
        uint8_t index_first  = indexes.at(i);
        uint8_t index_second = indexes.at(i + 1);
        uint8_t index_third  = indexes.at(i + 2);

        Vertex vertex_first  = program->VertexShader(vertices.at(index_first));
        Vertex vertex_second = program->VertexShader(vertices.at(index_second));
        Vertex vertex_third  = program->VertexShader(vertices.at(index_third));

        std::vector<Vertex> vertices_interpolated;
        RasterizeTriangle(
            vertex_first, vertex_second, vertex_third, vertices_interpolated);

        for (auto vertex : vertices_interpolated)
        {
            Color c = program->FragmentShader(vertex);

            Position pos{ static_cast<int>(std::round(vertex.position.x)),
                          static_cast<int>(std::round(vertex.position.y)) };
            SetPixel(pos, c);
        }
    }
}

void Program::SetUniforms(Uniform& uniform)
{
    mouse_x = uniform.u0;
    mouse_y = uniform.u1;
    radius  = uniform.u2;
}

Vertex Program::VertexShader(Vertex& vertex)
{
    Vertex v_out = vertex;
    double dx    = vertex.position.x - mouse_x;
    double dy    = vertex.position.y - mouse_y;
    if (sqrt(dx * dx + dy * dy) < radius)
    {
        double ratio   = sqrt(dx * dx + dy * dy) / radius;
        double ratio_x = 1 - ratio;
        double ratio_y = 1 - ratio;
        v_out.position.x -= dx * ratio_x;
        v_out.position.y -= dy * ratio_y;
    }
    return v_out;
}

Color Program::FragmentShader(Vertex& vertex)
{
    Color c_out = vertex.color;
    int   dx    = vertex.position.x - mouse_x;
    int   dy    = vertex.position.y - mouse_y;
    if (sqrt(dx * dx + dy * dy) < radius)
    {
        c_out.red   = 255 - c_out.red;
        c_out.green = 255 - c_out.green;
        c_out.blue  = 255 - c_out.blue;
    }
    return c_out;
}
