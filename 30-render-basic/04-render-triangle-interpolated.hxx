#pragma once

#include "03-render-triangle-indexed.hxx"

class Pixel;
class RenderTriangleInterpolated;

class Position3D
{
    friend class Pixel;
    friend class RenderTriangleInterpolated;

    double x;
    double y;
    double z;

public:
    Position3D();
    Position3D(double x, double y, double z);
    friend Position3D interpolate(const Position3D& first,
                                  const Position3D& second,
                                  const double      t);
};

class Pixel
{
    friend class RenderTriangleInterpolated;

    Position3D position;
    Color      color;

public:
    Pixel();
    Pixel(Position3D p, Color c);
    friend Pixel interpolate(const Pixel& first,
                             const Pixel& second,
                             const double t);
};

class RenderTriangleInterpolated : public RenderTriangleIndexed
{
    void RasterizeHorizontalLine(Pixel&              left,
                                 Pixel&              right,
                                 std::vector<Pixel>& pixels);
    void RasterizeHorizontalTriangle(Pixel&              vertex_left,
                                     Pixel&              vertex_right,
                                     Pixel&              vertex_single,
                                     std::vector<Pixel>& pixels);
    void RasterizeTriangle(Pixel&              vertex_first,
                           Pixel&              vertex_second,
                           Pixel&              vertex_third,
                           std::vector<Pixel>& pixels);

public:
    RenderTriangleInterpolated(Canvas& c);
    void DrawTriangle(std::vector<Pixel>&   vertices,
                      std::vector<uint8_t>& indexes);
};
