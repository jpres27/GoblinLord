#pragma once

// 
// COMPILERS
// 

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#if COMPILER_MSVC
#include <intrin.h>
#endif

#define kilobytes(value) ((value) * 1024)
#define megabytes(value) (kilobytes(value) * 1024)
#define gigabytes(value) (megabytes(value) * 1024)
#define terabytes(value) (gigabytes(value) * 1024)

#define array_count(array) (sizeof(array) / sizeof((array)[0]))

inline u32 safe_truncate_uint64(u64 value)
{
    // TODO: Defines for maximum values
    assert(value <= 0xFFFFFFFF);
    u32 value_32 = (u32)value; 
    return value_32;
}

struct Thread_Context 
{
    int placeholder;
};

struct Game_Offscreen_Buffer
{
    // NOTE: Pixels are always 32 bits wide, memory order BB GG RR XX
    void *memory;
    int width;
    int height;
    int pitch;
    int bpp;
};

struct Game_Button_State
{
    i32 half_transistion_count;
    b32 ended_down;

};

struct Game_Controller_Input
{
    b32 is_connected;
    Game_Button_State mouse_buttons[2];
    i32 mouse_x, mouse_y, mouse_z;

    union 
    {
        Game_Button_State buttons[10];
        struct 
        {
            Game_Button_State move_fwd;
            Game_Button_State move_back;
            Game_Button_State move_left;
            Game_Button_State move_right;

            Game_Button_State attack;
            Game_Button_State block;
            Game_Button_State inventory;
            Game_Button_State journal;
            Game_Button_State use;
            Game_Button_State jump;

            // NOTE: All buttons must be added above this line for bounds checking
            Game_Button_State terminator;
        };
    };
    r32 dt_for_frame;
};

struct Game_Memory
{
    b32 is_initialized;

    u64 total_storage_size;
    void *game_memory_block;

    u64 permanent_storage_size;
    void *permanent_storage; // NOTE: Since this is allocated with VitualAlloc, it is automatically
                             //       cleared to zero at startup
    u64 transient_storage_size;
    void *transient_storage;
};

struct Game_State
{

};