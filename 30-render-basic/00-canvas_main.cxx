#include "00-canvas.hxx"

#include <cstdlib>

int main(int argc, char** argv)
{
    const Color green = { 0, 255, 0 };

    size_t width  = 800;
    size_t height = 600;

    Canvas image(width, height);

    std::fill(image.Begin(), image.End(), green);

    std::string filename = "green_background.ppm";

    if (image.SaveImage(filename))
        return EXIT_FAILURE;

    Canvas loaded_image(0, 0);
    if (loaded_image.LoadImage(filename))
        return EXIT_FAILURE;

    if (image == loaded_image)
        std::cout << "image == loaded_image" << std::endl;
    else
    {
        std::cerr << "image != loaded_image" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
