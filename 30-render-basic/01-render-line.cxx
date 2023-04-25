#include "01-render-line.hxx"

RenderLine::RenderLine(Canvas& c)
    : canvas(c)
{
}

void RenderLine::Clear(Color c)
{
    std::fill(canvas.Begin(), canvas.End(), c);
}

void RenderLine::SetPixel(Position p, Color c)
{
    canvas.SetPixel(p.GetX(), p.GetY(), c);
}

Pixels RenderLine::GetPositions(Position start, Position end)
{
    Pixels line;
    int    x0 = start.GetX();
    int    y0 = start.GetY();
    int    x1 = end.GetX();
    int    y1 = end.GetY();

    bool incline = abs(x0 - x1) > abs(y0 - y1) ? 1 : 0;

    int error = 0;

    if (incline)
    {
        if (x0 > x1)
        {
            std::swap(y0, y1);
            std::swap(x0, x1);
        }

        int dx = x1 - x0;
        int dy = y1 - y0;
        int dir_y;

        if (dy < 0)
        {
            dir_y = -1;
            dy *= -1;
        }
        else
            dir_y = 1;

        int y = y0;

        for (int x = x0; x <= x1; x++)
        {
            line.push_back({ x, y });
            if (error > dx)
            {
                y += dir_y;
                error -= dx;
            }
            error += dy;
        }
    }
    else
    {
        if (y0 > y1)
        {
            std::swap(y0, y1);
            std::swap(x0, x1);
        }

        int dx = x1 - x0;
        int dy = y1 - y0;
        int dir_x;

        if (dx < 0)
        {
            dir_x = 1;
            dx *= -1;
        }
        else
            dir_x = 1;

        int x = x0;

        for (int y = y0; y <= y1; y++)
        {
            line.push_back({ x, y });
            if (error > dy)
            {
                x += dir_x;
                error -= dy;
            }
            error += dx;
        }
    }

    return line;
}

void RenderLine::DrawBresenhamLine(Position start, Position end, Color c)
{
    Pixels line = GetPositions(start, end);
    for (auto pixel : line)
        SetPixel(pixel, c);
}
