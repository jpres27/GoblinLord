#pragma once

#define KEY_MESSAGE_WAS_DOWN_BIT (1 << 30)
#define KEY_MESSAGE_IS_DOWN_BIT (1 << 31)
#define ALT_KEY_DOWN_BIT (1 << 29)

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

struct Win32_Buffer
{
    // NOTE: Pixels are always 32 bits wide, memory order BB GG RR XX
    BITMAPINFO info;
    void *memory;
    int width;
    int height;
    int pitch;
    int bpp;
};

struct Win32_Window_Dimensions
{
    int width;
    int height;
};