internal RGBA8 to_rgba8(v4 *color)
{
	RGBA8 result;

	result.r = max(0.f, min(255.f, color->X * 255.f));
	result.g = max(0.f, min(255.f, color->Y * 255.f));
	result.b = max(0.f, min(255.f, color->Z * 255.f));
	result.a = max(0.f, min(255.f, color->W * 255.f));

	return result;
}

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

internal void ClearScreen(Game_Offscreen_Buffer *buffer)
{
	v2 min = V2(0.0f, 0.0f);
	v2 max = V2((r32)buffer->width, (r32)buffer->height);
	DrawRectangle(buffer, min, max, 1.0f, 0.984f, 0.0f);
}

internal RGBA8 GetPixel(Game_Offscreen_Buffer *buffer, u32 x, u32 y)
{
    u8 *row = (u8 *)buffer->memory;
	row += ((y*buffer->width + x)*buffer->bpp);
    RGBA8 pixel = *((RGBA8 *)row);
    
    return(pixel);
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

internal void DrawLine(Game_Offscreen_Buffer *buffer, i32 x0, i32 y0, i32 x1, i32 y1, r32 r, r32 g, r32 b)
{
	// TODO: Horizontal, vertical, and diagonal lines can be special cased, according to Mike Abrash,
	// as they are much cheaper to do than the general line

	b32 steep = false;
	if(AbsoluteValue(x0 - x1) < AbsoluteValue(y0 - y1))
	{
		i32 temp = x0;
		x0 = y0;
		y0 = temp;
		temp = x1;
		x1 = y1;
		y1 = temp;
		steep = true;
	}

	if(x0 > x1)
	{
		i32 temp = x0;
		x0 = x1;
		x1 = temp;
		temp = y0;
		y0 = y1;
		y1 = temp;
	}

	i32 delta_x = x1 - x0;
	i32 delta_y = y1 - y0;
	i32 delta_error_x2 = AbsoluteValue(delta_y)*2;
	i32 error_x2 = 0;

	i32 y = y0;
	for(i32 x = x0; x <= x1; ++x) 
	{
		if(steep) 
		{
			DrawPixel(buffer, y, x, r, g, b);
		}
		else 
		{
			DrawPixel(buffer, x, y, r, g, b);
		}
		error_x2 += delta_error_x2;
		if(error_x2 > delta_x) 
		{
			y += (y1 > y0 ? 1 : -1);
			error_x2 -= delta_x*2;
		}
	}
}