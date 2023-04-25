#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

class Color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

public:
    Color()
        : red(0)
        , green(0)
        , blue(0)
    {
    }
    Color(uint8_t r, uint8_t g, uint8_t b)
        : red(r)
        , green(g)
        , blue(b)
    {
    }
    bool operator==(const Color& other) const
    {
        return red == other.red && green == other.green && blue == other.blue;
    }
};

class Canvas
{
    std::size_t        width;
    std::size_t        height;
    std::vector<Color> pixels;

public:
    Canvas(std::size_t w, std::size_t h)
        : width(w)
        , height(h)
    {
        pixels.resize(width * height);
    }
    bool SaveImage(std::string& filename)
    {
        std::ofstream image;
        image.exceptions(std::ofstream::failbit);
        try
        {
            image.open(filename, std::ofstream::binary);
            image << "P6\n" << width << ' ' << height << '\n' << 255 << '\n';

            std::streamsize bufer_size =
                static_cast<std::streamsize>(pixels.size() * sizeof(Color));
            image.write(reinterpret_cast<const char*>(pixels.data()),
                        bufer_size);

            image.close();
        }
        catch (std::ofstream::failure fail)
        {
            std::cerr << "Exception opening/writing/closing file" << std::endl;
            return 1;
        }
        return 0;
    }
    bool LoadImage(std::string& filename)
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
    auto Begin() { return pixels.begin(); }
    auto End() { return pixels.end(); }
    bool operator==(const Canvas& other) const
    {
        return pixels == other.pixels;
    }
};
