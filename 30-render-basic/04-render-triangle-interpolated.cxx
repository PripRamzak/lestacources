#include "04-render-triangle-interpolated.hxx"

#include <cmath>

Position3D interpolate(const Position3D& first,
                       const Position3D& second,
                       const double      t)
{
    return { first.x + second.x * t - first.x * t,
             first.y + second.y * t - first.y * t,
             first.z + second.z * t - first.z * t };
}

Color interpolate(const Color& first, const Color& second, const double t)
{
    return { static_cast<uint8_t>(
                 std::round(first.red + second.red * t - first.red * t)),
             static_cast<uint8_t>(
                 std::round(first.green + second.green * t - first.green * t)),
             static_cast<uint8_t>(
                 std::round(first.blue + second.blue * t - first.blue * t)) };
}

Vertex interpolate(const Vertex& first, const Vertex& second, const double t)
{
    return { interpolate(first.position, second.position, t),
             interpolate(first.color, second.color, t) };
}

RenderTriangleInterpolated::RenderTriangleInterpolated(Canvas& c)
    : RenderTriangleIndexed(c)
{
}

void RenderTriangleInterpolated::RasterizeHorizontalLine(
    Vertex& left, Vertex& right, std::vector<Vertex>& pixels)
{
    int pixels_count = std::round(right.position.x - left.position.x);

    if (pixels_count > 0)
        for (int x = 0; x <= pixels_count + 1; x++)
        {
            double t         = static_cast<double>(x) / (pixels_count + 1);
            Vertex pixel_new = interpolate(left, right, t);
            pixels.push_back(pixel_new);
        }
    else
        pixels.push_back(left);
}

void RenderTriangleInterpolated::RasterizeHorizontalTriangle(
    Vertex&              vertex_left,
    Vertex&              vertex_right,
    Vertex&              vertex_single,
    std::vector<Vertex>& pixels)
{
    if (vertex_single.position.y == vertex_left.position.y)
    {
        if (vertex_single.position.x < vertex_left.position.x)
            RasterizeHorizontalLine(vertex_single, vertex_right, pixels);
        else if (vertex_single.position.x > vertex_right.position.x)
            RasterizeHorizontalLine(vertex_left, vertex_single, pixels);
        else
            RasterizeHorizontalLine(vertex_left, vertex_right, pixels);
    }
    else
    {
        int y_step = vertex_single.position.y < vertex_left.position.y ? 1 : -1;
        int pixels_count =
            (std::abs(vertex_single.position.y - vertex_left.position.y));
        int border = vertex_left.position.y;
        for (int y = 0; std::abs(y) <= pixels_count + 1; y += y_step)
        {
            double t    = static_cast<double>(std::abs(y)) / (pixels_count + 1);
            Vertex left = interpolate(vertex_left, vertex_single, t);
            Vertex right = interpolate(vertex_right, vertex_single, t);
            RasterizeHorizontalLine(left, right, pixels);
        }
    }
}

void RenderTriangleInterpolated::RasterizeTriangle(Vertex& vertex_first,
                                                   Vertex& vertex_second,
                                                   Vertex& vertex_third,
                                                   std::vector<Vertex>& pixels)
{
    Vertex mid, vertex_first_new, vertex_second_new;

    auto find_mid =
        [](Vertex& vertex_first, Vertex& vertex_second, Vertex& vertex_third)
    {
        return (vertex_first.position.y > vertex_second.position.y &&
                vertex_first.position.y < vertex_third.position.y) ||
               (vertex_first.position.y < vertex_second.position.y &&
                vertex_first.position.y > vertex_third.position.y);
    };

    auto find_left = [](Vertex& vertex_first, Vertex& vertex_second)
    { return vertex_first.position.x < vertex_second.position.x; };

    if (vertex_first.position.y == vertex_second.position.y ==
        vertex_third.position.y)
    {
        if (find_left(vertex_first, vertex_second))
        {
            if (find_left(vertex_first, vertex_third))
            {
                if (find_left(vertex_second, vertex_third))
                    RasterizeHorizontalLine(vertex_first, vertex_third, pixels);
                else
                    RasterizeHorizontalLine(
                        vertex_first, vertex_second, pixels);
            }
            else
                RasterizeHorizontalLine(vertex_third, vertex_second, pixels);
        }
        else
        {
            if (find_left(vertex_second, vertex_third))
            {
                if (find_left(vertex_third, vertex_first))
                    RasterizeHorizontalLine(
                        vertex_second, vertex_first, pixels);
                else
                    RasterizeHorizontalLine(
                        vertex_second, vertex_third, pixels);
            }
            else
                RasterizeHorizontalLine(vertex_third, vertex_second, pixels);
        }
        return;
    }
    if (find_mid(vertex_first, vertex_second, vertex_third) ||
        vertex_second.position.y == vertex_third.position.y)
    {
        mid               = vertex_first;
        vertex_first_new  = vertex_second;
        vertex_second_new = vertex_third;
    }
    else if (find_mid(vertex_second, vertex_first, vertex_third) ||
             vertex_first.position.y == vertex_third.position.y)
    {
        mid               = vertex_second;
        vertex_first_new  = vertex_first;
        vertex_second_new = vertex_third;
    }
    else if (find_mid(vertex_third, vertex_first, vertex_second) ||
             vertex_first.position.y == vertex_second.position.y)
    {
        mid               = vertex_third;
        vertex_first_new  = vertex_first;
        vertex_second_new = vertex_second;
    }

    if (vertex_first_new.position.y == vertex_second_new.position.y)
    {
        if (find_left(vertex_first_new, vertex_second_new))
            RasterizeHorizontalTriangle(
                vertex_first_new, vertex_second_new, mid, pixels);
        else
            RasterizeHorizontalTriangle(
                vertex_second_new, vertex_first_new, mid, pixels);
    }
    else
    {
        double t = static_cast<double>(
            std::abs(mid.position.y - vertex_first_new.position.y) /
            std::abs(vertex_first_new.position.y -
                     vertex_second_new.position.y));

        Vertex second_mid = interpolate(vertex_first_new, vertex_second_new, t);

        if (find_left(mid, second_mid))
        {
            RasterizeHorizontalTriangle(
                mid, second_mid, vertex_first_new, pixels);
            RasterizeHorizontalTriangle(
                mid, second_mid, vertex_second_new, pixels);
        }
        else
        {
            RasterizeHorizontalTriangle(
                second_mid, mid, vertex_first_new, pixels);
            RasterizeHorizontalTriangle(
                second_mid, mid, vertex_second_new, pixels);
        }
    }
}

void RenderTriangleInterpolated::DrawTriangles(std::vector<Vertex>&  vertices,
                                               std::vector<uint8_t>& indexes)
{
    for (size_t i = 0; i < indexes.size(); i += 3)
    {
        uint8_t index_first  = indexes.at(i);
        uint8_t index_second = indexes.at(i + 1);
        uint8_t index_third  = indexes.at(i + 2);

        Vertex vertex_first  = vertices.at(index_first);
        Vertex vertex_second = vertices.at(index_second);
        Vertex vertex_third  = vertices.at(index_third);

        std::vector<Vertex> pixels_interpolated;

        RasterizeTriangle(
            vertex_first, vertex_second, vertex_third, pixels_interpolated);

        for (Vertex& interpolated_pixel : pixels_interpolated)
        {
            Position pos{
                static_cast<int>(std::round(interpolated_pixel.position.x)),
                static_cast<int>(std::round(interpolated_pixel.position.y))
            };
            SetPixel(pos, interpolated_pixel.color);
        }
    }
}

GFXProgram::~GFXProgram(){};
