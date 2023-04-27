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

    Pixel v1{ { 10, 10, 0 }, red };
    Pixel v2{ { 540, 270, 0 }, green };
    Pixel v3{ { 90, 380, 0 }, blue };

    std::vector<Pixel>   vertices{ v1, v2, v3 };
    std::vector<uint8_t> indexes{ 0, 1, 2 };

    render.DrawTriangle(vertices, indexes);

    std::string filename = "04-interpolated-triangle.ppm";
    if (image.SaveImage(filename))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
