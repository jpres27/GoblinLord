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