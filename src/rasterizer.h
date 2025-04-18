#pragma once

struct RGBA8
{
    u8 r, g, b, a;
};

struct RGBAReal32
{
    r32 red, green, blue, alpha;
};

enum CullMode
{
    none,
    cw,
    ccw
};

struct Mesh
{
    v3 *positions;
    u32 num_vertices;
    RGBAReal32 colorf; 
};

struct DrawCommand
{
    Mesh mesh;
    CullMode cull_mode;
    m4x4 transform;
};