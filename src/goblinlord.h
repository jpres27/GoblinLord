#pragma once

#define internal static
#define local_persist static
#define global_variable static

#define assert(expression) if(expression == false) {*(int *)0 = 0;}
#define AssertHR(hr) assert(SUCCEEDED(hr))
#define array_count(array) (sizeof(array) / sizeof((array)[0]))

#define Min(a, b) ((a < b) ? (a) : (b))
#define Max(a, b) ((a > b) ? (a) : (b))

typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t b32;
typedef float r32;
typedef double r64;
typedef size_t memory_index;

global_variable r32 PI = 3.141592;

struct Game_Clocks 
{
    r32 second_elapsed;
};

struct Memory_Arena 
{
    memory_index size;
    uint8_t *base;
    memory_index used;
};

#define push_struct(arena, type) (type *)push_size_(arena, sizeof(type))
#define push_array(arena, count, type) (type *)push_size_(arena, (count)*sizeof(type))
void * push_size_(Memory_Arena *arena, memory_index size)
{
    assert((arena->used + size) < arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;
    return(result);
}

struct Loaded_Bitmap
{
    int32_t pixel_width;
    int32_t pixel_height;
    uint32_t *pixels;
};
