internal void DrawRectangle(Game_Offscreen_Buffer *buffer, v2 v_min, v2 v_max, r32 red, r32 green, r32 blue)
{        
	i32 min_x = RoundReal32ToInt32(v_min.X);
    i32 min_y = RoundReal32ToInt32(v_min.Y);
    i32 max_x = RoundReal32ToInt32(v_max.X);
    i32 max_y = RoundReal32ToInt32(v_max.Y);

    if(min_x < 0)
    {
        min_x = 0;
    }

    if(min_y < 0)
    {
        min_y = 0;
    }

    if(max_x > buffer->width)
    {
        max_x = buffer->width;
    }

    if(max_y > buffer->height)
    {
        max_y = buffer->height;
    }
    /*
        Pixel in memory: BB GG RR xx
    */

    u32 color = ((RoundReal32ToUInt32(red * 255.0f) << 16) |
                 (RoundReal32ToUInt32(green * 255.0f) << 8) |
                 (RoundReal32ToUInt32(blue * 255.0f) << 0));

    u8 *row = ((u8 *)buffer->memory + min_x*buffer->bpp + min_y*buffer->pitch);

    for(int y = min_y; y < max_y; ++y)
    {
        u32 *pixel = (u32 *)row;
        for(int x = min_x; x < max_x; ++x)
        {            
            *pixel++ = color;
        }
        
        row += buffer->pitch;
    }
}

internal void DrawPixel(Game_Offscreen_Buffer *buffer, u32 x, u32 y, r32 red, r32 green, r32 blue)
{    
    /*
        Pixel in memory: BB GG RR xx
    */
    u32 color = ((RoundReal32ToUInt32(red * 255.0f) << 16) |
                 (RoundReal32ToUInt32(green * 255.0f) << 8) |
                 (RoundReal32ToUInt32(blue * 255.0f) << 0));

	u8 *row = (u8 *)buffer->memory;
	row += ((y*buffer->width + x)*buffer->bpp);
    u32 *pixel = (u32 *)row;
	*pixel = color;
}

internal void GameUpdateAndRender(Game_Offscreen_Buffer *buffer, Game_Controller_Input *input, v2 dd_player) 
{
	v2 min = V2(0.0f, 0.0f);
	v2 max = V2((r32)buffer->width, (r32)buffer->height);
	DrawRectangle(buffer, min, max, 1.0f, 0.984f, 0.0f);

	DrawPixel(buffer, buffer->width/2, buffer->height/2, 0, 255, 0);
	DrawPixel(buffer, (buffer->width/2)+1, (buffer->height/2)+1, 0.91f, 0.141f, 0.376f);
	DrawPixel(buffer, (buffer->width/2)+2, (buffer->height/2)+2, 0.91f, 0.141f, 0.376f);
	DrawPixel(buffer, (buffer->width/2)+3, (buffer->height/2)+3, 0.91f, 0.141f, 0.376f);
}
