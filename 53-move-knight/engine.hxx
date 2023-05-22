#pragma once

#include "event.hxx"
#include "geometry.hxx"

#include <iostream>
#include <vector>

class engine
{
public:
    virtual bool  initialize()                                             = 0;
    virtual bool  read_input(event& e)                                     = 0;
    virtual float get_time()                                               = 0;
    virtual void  clear()                                                  = 0;
    virtual void  set_uniform(const char* name, float value)               = 0;
    virtual bool  load_texture()                                           = 0;
    virtual void  move_texture(std::vector<triangle_2d>& texture_triangles,
                               float                     dx,
                               float                     dy,
                               int&                      direction)                             = 0;
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
