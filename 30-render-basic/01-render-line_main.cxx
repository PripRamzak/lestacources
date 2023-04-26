#include "01-render-line.hxx"

#include <cstdlib>

int main(int argc, char** argv)
{
    const Color black{ 0, 0, 0 };

    size_t width  = 800;
    size_t height = 600;

    Canvas image{ width, height };

    RenderLine render(image);
    render.Clear(black);

    Position start, end;
    Color    color;

    for (int i = 0; i < 50; i++)
    {
        start = { Position::GenerateRandom(width, height) };
        end   = { Position::GenerateRandom(width, height) };

        color = { static_cast<uint8_t>(rand() % 256),
                  static_cast<uint8_t>(rand() % 256),
                  static_cast<uint8_t>(rand() % 256) };

        render.DrawBresenhamLine(start, end, color);
    }

    std::string filename = "01-lines.ppm";

    if (image.SaveImage(filename))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
