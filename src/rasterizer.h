#pragma once

struct RGBA8
{
    u8 r, g, b, a;
};

struct Mesh
{
    v3 *positions;
    u32 num_vertices;
    v4 color; 
};