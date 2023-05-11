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

struct vertex
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

struct triangle
{
    triangle(vertex v1, vertex v2, vertex v3)
    {
        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
    }
    vertex vertices[3];
};

std::ostream& operator<<(std::ostream& out, const event& event);

class engine
{
public:
    virtual bool initialize()                                         = 0;
    virtual bool read_input(event& e)                                 = 0;
    virtual void clear()                                              = 0;
    virtual void set_uniforms(const char* name, float value)          = 0;
    virtual void generate_triangles(std::vector<triangle>& triangles) = 0;
    virtual void draw_triangles(const triangle& triangle)             = 0;
    virtual bool swap_bufers()                                        = 0;
    virtual void uninitialize()                                       = 0;
    virtual ~engine();
};

engine* create_engine();
void    destroy_engine(engine* engine);
