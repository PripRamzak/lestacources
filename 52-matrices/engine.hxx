#pragma once

#include <iostream>
#include <vector>

enum class event
{
    up_pressed,
    up_released,
    down_pressed,
    down_released,
    left_pressed,
    left_released,
    right_pressed,
    right_released,
    button1_pressed,
    button1_released,
    button2_pressed,
    button2_released,
    select_pressed,
    select_released,
    start_pressed,
    start_released,
    turn_off,
    null_event
};

std::ostream& operator<<(std::ostream& out, const event& event);

struct vertex_2d
{
    float x = 0.f;
    float y = 0.f;
    float u = 0.f;
    float v = 0.f;
};

std::istream& operator>>(std::istream& is, vertex_2d& vertex);

struct vertex
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

vertex interpolate(vertex& v1, vertex& v2, float alpha);

std::istream& operator>>(std::istream& is, vertex& vertex);

struct triangle_2d
{
    vertex_2d vertices[3];

    triangle_2d()
    {
        vertices[0] = { 0.f, 0.f, 0.f, 0.f };
        vertices[0] = { 0.f, 0.f, 0.f, 0.f };
        vertices[0] = { 0.f, 0.f, 0.f, 0.f };
    };

    triangle_2d(vertex_2d v1, vertex_2d v2, vertex_2d v3)
    {
        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
    }
};

std::istream& operator>>(std::istream& is, triangle_2d& triangle);

struct triangle
{
    vertex vertices[3];

    triangle()
    {
        vertices[0] = { 0.f, 0.f, 0.f };
        vertices[1] = { 0.f, 0.f, 0.f };
        vertices[2] = { 0.f, 0.f, 0.f };
    }

    triangle(vertex v1, vertex v2, vertex v3)
    {
        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
    }
};

triangle transform(triangle& t1, triangle& t2, float alpha);

std::istream& operator>>(std::istream& is, triangle& triangle);

class engine
{
public:
    virtual bool  initialize()                                             = 0;
    virtual bool  read_input(event& e)                                     = 0;
    virtual float get_time()                                               = 0;
    virtual void  clear()                                                  = 0;
    virtual void  set_uniform(const char* name, float value)               = 0;
    virtual bool  load_texture()                                           = 0;
    virtual void  generate_triangles(std::vector<triangle>& triangles)     = 0;
    virtual void  draw_triangles_2d(const triangle_2d& triangle)           = 0;
    virtual void  draw_triangles(const triangle& triangle)                 = 0;
    virtual void  generate_circle(std::vector<triangle>& circle_triangles) = 0;
    virtual bool  swap_buffers()                                           = 0;
    virtual void  uninitialize()                                           = 0;
    virtual ~engine();
};

engine* create_engine();

void destroy_engine(engine* engine);
