#include "04-render-triangle-interpolated.hxx"

#include <cmath>

Position3D::Position3D()
    : x(0)
    , y(0)
    , z(0)
{
}

Position3D::Position3D(double x, double y, double z)
    : x(x)
    , y(y)
    , z(z)
{
}

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

Pixel::Pixel()
    : color(Color())
    , position(Position3D())
{
}

Pixel::Pixel(Position3D p, Color c)
    : position(p)
    , color(c)
{
}

Pixel interpolate(const Pixel& first, const Pixel& second, const double t)
{
    return { interpolate(first.position, second.position, t),
             interpolate(first.color, second.color, t) };
}

RenderTriangleInterpolated::RenderTriangleInterpolated(Canvas& c)
    : RenderTriangleIndexed(c)
{
}

void RenderTriangleInterpolated::RasterizeHorizontalLine(
    Pixel& left, Pixel& right, std::vector<Pixel>& pixels)
{
    int pixels_count = std::round(right.position.x - left.position.x);

    if (pixels_count > 0)
        for (int i = 0; i < pixels_count; i++)
        {
            double t         = static_cast<double>(i) / (pixels_count + 1);
            Pixel  pixel_new = interpolate(left, right, t);
            pixels.push_back(pixel_new);
        }
    else
        pixels.push_back(left);
}

void RenderTriangleInterpolated::RasterizeHorizontalTriangle(
    Pixel&              vertex_left,
    Pixel&              vertex_right,
    Pixel&              vertex_single,
    std::vector<Pixel>& pixels)
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
        for (int y = vertex_single.position.y; y != border; y += y_step)
        {
            double t =
                static_cast<double>(std::abs(y - vertex_single.position.y)) /
                (pixels_count + 1);
            Pixel left  = interpolate(vertex_left, vertex_single, t);
            Pixel right = interpolate(vertex_right, vertex_single, t);
            RasterizeHorizontalLine(left, right, pixels);
        }
    }
}

void RenderTriangleInterpolated::RasterizeTriangle(Pixel& vertex_first,
                                                   Pixel& vertex_second,
                                                   Pixel& vertex_third,
                                                   std::vector<Pixel>& pixels)
{
    Pixel mid, vertex_first_new, vertex_second_new;

    auto find_mid =
        [](Pixel& vertex_first, Pixel& vertex_second, Pixel& vertex_third)
    {
        return (vertex_first.position.y > vertex_second.position.y &&
                vertex_first.position.y < vertex_third.position.y) ||
               (vertex_first.position.y < vertex_second.position.y &&
                vertex_first.position.y > vertex_third.position.y);
    };

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

    auto find_left = [](Pixel& vertex_first, Pixel& vertex_second)

    { return vertex_first.position.x < vertex_second.position.x; };

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

        Pixel second_mid = interpolate(vertex_first_new, vertex_second_new, t);

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

void RenderTriangleInterpolated::DrawTriangle(std::vector<Pixel>&   vertices,
                                              std::vector<uint8_t>& indexes)
{
    for (size_t i = 0; i < indexes.size(); i += 3)
    {
        uint8_t index_first  = indexes.at(i);
        uint8_t index_second = indexes.at(i + 1);
        uint8_t index_third  = indexes.at(i + 2);

        Pixel vertex_first  = vertices.at(index_first);
        Pixel vertex_second = vertices.at(index_second);
        Pixel vertex_third  = vertices.at(index_third);

        std::vector<Pixel> pixels_interpolated;

        RasterizeTriangle(
            vertex_first, vertex_second, vertex_third, pixels_interpolated);

        for (Pixel& interpolated_pixel : pixels_interpolated)
        {
            Position pos{
                static_cast<int>(std::round(interpolated_pixel.position.x)),
                static_cast<int>(std::round(interpolated_pixel.position.y))
            };
            SetPixel(pos, interpolated_pixel.color);
        }
    }
}
