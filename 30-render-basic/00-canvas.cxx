#include "00-canvas.hxx"

#include <algorithm>
#include <iostream>

bool operator==(const Color& color_first, const Color& color_second)
{
    return color_first.red == color_second.red &&
           color_first.green == color_second.green &&
           color_first.blue == color_second.blue;
}

Color Color::GenerateRandom()
{
    return { static_cast<uint8_t>(std::rand() % 256),
             static_cast<uint8_t>(std::rand() % 256),
             static_cast<uint8_t>(std::rand() % 256) };
}

Canvas::Canvas()
    : width(0)
    , height(0)
{
}

Canvas::Canvas(std::size_t w, std::size_t h)
    : width(w)
    , height(h)
{
    pixels.resize(width * height);
}

bool Canvas::SaveImage(std::string& filename)
{
    std::ofstream image;
    image.exceptions(std::ofstream::failbit);
    try
    {
        image.open(filename, std::ofstream::binary);
        image << "P6\n" << width << ' ' << height << '\n' << 255 << '\n';

        std::streamsize bufer_size =
            static_cast<std::streamsize>(pixels.size() * sizeof(Color));
        image.write(reinterpret_cast<const char*>(pixels.data()), bufer_size);

        image.close();
    }
    catch (std::ofstream::failure fail)
    {
        std::cerr << "Exception opening/writing/closing file" << std::endl;
        return 1;
    }
    return 0;
}

bool Canvas::LoadImage(std::string& filename)
{
    std::ifstream image;
    image.exceptions(std::ifstream::failbit);
    try
    {
        image.open(filename, std::ifstream::binary);

        std::string file_format, color_format;
        char        whitespace;
        image >> file_format >> width >> height >> color_format >>
            std::noskipws >> whitespace;

        if (!iswspace(whitespace))
        {
            std::cerr << "Expected whitespace" << std::endl;
            return 1;
        }

        pixels.resize(width * height);
        if (pixels.size() != width * height)
        {
            std::cerr << "Image size not match" << std::endl;
            return 1;
        }

        std::streamsize bufer_size =
            static_cast<std::streamsize>(pixels.size() * sizeof(Color));
        image.read(reinterpret_cast<char*>(pixels.data()), bufer_size);

        image.close();
    }
    catch (std::ifstream::failure)
    {
        std::cerr << "Exception opening/reading/closing file" << std::endl;
        return 1;
    }
    return 0;
}

void Canvas::SetPixel(size_t x, size_t y, Color& c)
{
    size_t pixel_number     = width * y + x;
    pixels.at(pixel_number) = c;
}

Color Canvas::GetPixel(size_t x, size_t y) const
{
    size_t pixel_number = width * y + x;
    return pixels.at(pixel_number);
}

std::vector<Color>& Canvas::GetPixels()
{
    return pixels;
}

std::vector<Color>::iterator Canvas::Begin()
{
    return pixels.begin();
}

std::vector<Color>::iterator Canvas::End()
{
    return pixels.end();
}

bool operator==(const Canvas& canvas_first, const Canvas& canvas_second)
{
    return canvas_first.pixels == canvas_second.pixels;
}

Position::Position()
    : x(0)
    , y(0)
{
}

Position::Position(int x, int y)
    : x(x)
    , y(y)
{
}

int Position::GetX()
{
    return x;
}

int Position::GetY()
{
    return y;
}

Position Position::GenerateRandom(int width, int height)
{
    return { rand() % width, rand() % height };
}

Position operator-(const Position& position_first,
                   const Position& position_second)
{
    return { position_first.x - position_second.x,
             position_first.y - position_second.y };
}

Render::~Render(){};
