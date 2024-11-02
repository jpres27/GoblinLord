internal void FillScreen(Game_Offscreen_Buffer *buffer, u8 blue, u8 green, u8 red)
{    
    /*
        Pixel in memory: BB GG RR xx
    */

    int width = buffer->width;
    int height = buffer->height;

    int pitch = width*buffer->bpp;
    u8 *row = (u8 *)buffer->memory;    
    for(int y = 0; y < height; ++y)
    {
        u8 *pixel = row;
        for(int x = 0; x < width; ++x)
        {
            *pixel = blue;
            ++pixel;
             
             *pixel = green;
             ++pixel;

            *pixel = red;
             ++pixel;

            *pixel = 0;
             ++pixel;
            }
        }

        row += pitch;
}

internal void DrawPixel(Game_Offscreen_Buffer *buffer, u32 x, u32 y, u8 blue, u8 green, u8 red)
{    
    /*
        Pixel in memory: BB GG RR xx
    */
    u8 *pixel = (u8 *)buffer->memory;
    pixel += (y*buffer->width+x)*buffer->bpp;
    *pixel = blue;
    ++pixel;
             
    *pixel = green;
    ++pixel;

    *pixel = red;
    ++pixel;
    
    *pixel = 0;
    ++pixel;
}

internal void GameUpdateAndRender(Game_Offscreen_Buffer *buffer, Game_Controller_Input *input, Vec2 dd_player) 
{
	FillScreen(buffer, 255, 0, 0);
}
