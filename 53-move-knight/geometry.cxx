#include "geometry.hxx"

std::istream& operator>>(std::istream& is, vertex_2d& vertex)
{
    is >> vertex.x >> vertex.y >> vertex.u >> vertex.v;
    return is;
}

std::istream& operator>>(std::istream& is, vertex& vertex)
{
    is >> vertex.x >> vertex.y >> vertex.z;
    return is;
}

vertex interpolate(vertex& v1, vertex& v2, float alpha)
{
    vertex v;
    v.x = (1.f - alpha) * v1.x + v2.x * alpha;
    v.y = (1.f - alpha) * v1.y + v2.y * alpha;
    v.z = (1.f - alpha) * v1.z + v2.z * alpha;
    return v;
}

std::istream& operator>>(std::istream& is, triangle_2d& triangle)
{
    is >> triangle.vertices[0] >> triangle.vertices[1] >> triangle.vertices[2];
    return is;
}

triangle transform(triangle& t1, triangle& t2, float alpha)
{
    vertex   v1 = interpolate(t1.vertices[0], t2.vertices[0], alpha);
    vertex   v2 = interpolate(t1.vertices[1], t2.vertices[1], alpha);
    vertex   v3 = interpolate(t1.vertices[2], t2.vertices[2], alpha);
    triangle t  = { v1, v2, v3 };
    return t;
}

std::istream& operator>>(std::istream& is, triangle& triangle)
{
    is >> triangle.vertices[0] >> triangle.vertices[1] >> triangle.vertices[2];
    return is;
}
