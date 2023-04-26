#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

class Color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

public:
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    bool friend operator==(const Color& color_first, const Color& color_second);
};

class Canvas
{
    std::size_t        width;
    std::size_t        height;
    std::vector<Color> pixels;

public:
    Canvas();
    Canvas(std::size_t w, std::size_t h);
    bool                         SaveImage(std::string& filename);
    bool                         LoadImage(std::string& filename);
    void                         SetPixel(size_t x, size_t y, Color& c);
    Color                        GetPixel(size_t x, size_t y) const;
    std::vector<Color>::iterator Begin();
    std::vector<Color>::iterator End();
    friend bool                  operator==(const Canvas& canvas_first,
                           const Canvas& canvas_second);
};

class Position
{
    int x;
    int y;

public:
    Position();
    Position(int x, int y);
    int             GetX();
    int             GetY();
    static Position GenerateRandom(int width, int height);
    friend Position operator-(const Position& position_first,
                              const Position& position_second);
    friend bool     operator==(const Position& position_first,
                           const Position& position_second);
};

using Pixels = std::vector<Position>;

class Render
{
public:
    virtual void   Clear(Color c)                             = 0;
    virtual void   SetPixel(Position p, Color c)              = 0;
    virtual Pixels GetPositions(Position start, Position end) = 0;
    virtual ~Render();
};
