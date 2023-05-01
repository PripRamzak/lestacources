#include "04-render-triangle-interpolated.hxx"

int main(int argc, char** argv)
{
    const Color black{ 0, 0, 0 };
    const Color blue{ 0, 0, 255 };
    const Color green{ 0, 255, 0 };
    const Color red{ 255, 0, 0 };

    size_t width  = 800;
    size_t height = 600;

    Canvas image(width, height);

    RenderTriangleInterpolated render(image);
    render.Clear(black);

    Vertex v1{ { 0, 0, 0 }, red };
    Vertex v2{ { 100, 0, 0 }, green };
    Vertex v3{ { 0, 100, 0 }, blue };

    Vertex v4{ { 200, 0, 0 }, red };
    Vertex v5{ { 200, 100, 0 }, blue };

    std::vector<Vertex>  vertices{ v1, v2, v3, v4, v5 };
    std::vector<uint8_t> indexes{ 0, 1, 2, 1, 3, 4 };

    render.DrawTriangles(vertices, indexes);

    std::string filename = "04-interpolated-triangle.ppm";
    if (image.SaveImage(filename))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
