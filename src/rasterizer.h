#pragma once

struct RGBA8
{
    u8 r, g, b, a;
};

enum CullMode
{
    none,
    cw,
    ccw
};

struct Viewport
{
 i32 x_min, x_max, y_min, y_max;
};

struct Mesh
{
    v3 *vertices;
    u32 num_vertices;
    RGBA32 *colors;
};

struct DrawCommand
{
    Mesh mesh;
    CullMode cull_mode;
    m4x4 transform;
};