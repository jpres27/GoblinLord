internal RGBA8 to_rgba8(v4 *color)
{
	RGBA8 result;

	result.r = max(0.f, min(255.f, color->X * 255.f));
	result.g = max(0.f, min(255.f, color->Y * 255.f));
	result.b = max(0.f, min(255.f, color->Z * 255.f));
	result.a = max(0.f, min(255.f, color->W * 255.f));

	return result;
}

// x_max and y_max are exclusive
inline v4 NDCToScreen(Viewport *viewport, v4 v)
{
	v.X = viewport->x_min + ((viewport->x_max - viewport->x_min) * (0.5f + (0.5f * v.X)));
	v.Y = viewport->y_min + ((viewport->y_max - viewport->y_min) * (0.5f - (0.5f * v.Y)));
	return v;
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
	DrawRectangle(buffer, min, max, 1.0f, 1.0f, 1.0f);
}

internal RGBA8 GetPixel(Game_Offscreen_Buffer *buffer, u32 x, u32 y)
{
    u8 *row = (u8 *)buffer->memory;
	row += ((y*buffer->width + x)*buffer->bpp);
    RGBA8 pixel = *((RGBA8 *)row);
    
    return(pixel);
}

internal void DrawPixel(Game_Offscreen_Buffer *buffer, u32 x, u32 y, RGBA32 colorf)
{    
    /*
        Pixel in memory: BB GG RR xx
    */
    u32 color = ((RoundReal32ToUInt32(colorf.red * 255.0f) << 16) |
                 (RoundReal32ToUInt32(colorf.green * 255.0f) << 8) |
                 (RoundReal32ToUInt32(colorf.blue * 255.0f) << 0));

	u8 *row = (u8 *)buffer->memory;
	row += ((y*buffer->width + x)*buffer->bpp);
    u32 *pixel = (u32 *)row;
	*pixel = color;
}

internal void DrawLine(Game_Offscreen_Buffer *buffer, i32 x0, i32 y0, i32 x1, i32 y1, RGBA32 colorf)
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
			DrawPixel(buffer, y, x, colorf);
		}
		else 
		{
			DrawPixel(buffer, x, y, colorf);
		}
		error_x2 += delta_error_x2;
		if(error_x2 > delta_x) 
		{
			y += (y1 > y0 ? 1 : -1);
			error_x2 -= delta_x*2;
		}
	}
}

void Draw(Game_Offscreen_Buffer *buffer, Viewport *viewport, DrawCommand *command, Game_Controller_Input *input)
{
    for (u32 i = 0; i+2 < command->mesh.num_vertices; i += 3)
    {
        v4 vert0 = command->transform * Point3DTo4D(command->mesh.vertices[i+0]);
        v4 vert1 = command->transform * Point3DTo4D(command->mesh.vertices[i+1]);
        v4 vert2 = command->transform * Point3DTo4D(command->mesh.vertices[i+2]);

		vert0 = NDCToScreen(viewport, vert0);
		vert1 = NDCToScreen(viewport, vert1);
		vert2 = NDCToScreen(viewport, vert2);

		RGBA32 color0 = command->mesh.colors[i+0];
        RGBA32 color1 = command->mesh.colors[i+1];
        RGBA32 color2 = command->mesh.colors[i+2];

		r32 det012 = Determinant2D(vert1 - vert0, vert2 - vert0);

		b32 ccw = det012 < 0.0f;

		switch (command->cull_mode)
		{
			case CullMode::none:
				break;
			case CullMode::cw:
				if(!ccw) continue;
				break;
			case CullMode::ccw:
				if(ccw) continue;
				break;
		}

		if (ccw)
		{
			v4 temp;
			temp = vert1;
			vert1 = vert2;
			vert2 = temp;
    		det012 = -det012;
		}

		i32 x_min = Max(viewport->x_min, 0);
		i32 x_max = Min(viewport->x_max, buffer->width) - 1;
		i32 y_min = Max(viewport->y_min, 0);
		i32 y_max = Min(viewport->y_max, buffer->height) - 1;

		x_min = Max(x_min, Min(FloorReal32ToInt32(vert0.X), Min(FloorReal32ToInt32(vert1.X), FloorReal32ToInt32(vert2.X))));
		x_max = Min(x_max, Max(FloorReal32ToInt32(vert0.X), Max(FloorReal32ToInt32(vert1.X), FloorReal32ToInt32(vert2.X))));
		y_min = Max(y_min, Min(FloorReal32ToInt32(vert0.Y), Min(FloorReal32ToInt32(vert1.Y), FloorReal32ToInt32(vert2.Y))));
		y_max = Min(y_max, Max(FloorReal32ToInt32(vert0.Y), Max(FloorReal32ToInt32(vert1.Y), FloorReal32ToInt32(vert2.Y))));

		x_min = Max(0, x_min);
		x_max = Min(buffer->width - 1, x_max);
		y_min = Max(0, y_min);
		y_max = Min(buffer->height - 1, y_max);

        for (i32 y = y_min; y <= y_max; ++y)
        {
            for (i32 x = x_min; x <= x_max; ++x)
            {
                v4 p = V4(x + 0.5f, y + 0.5f, 0.f, 0.f);

                r32 det01p = Determinant2D(vert1-vert0, p-vert0);
                r32 det12p = Determinant2D(vert2-vert1, p-vert1);
                r32 det20p = Determinant2D(vert0-vert2, p-vert2);

                if (det01p >= 0.0f && det12p >= 0.0f && det20p >= 0.0f)
				{
    				r32 l0 = det12p / det012;
    				r32 l1 = det20p / det012;
    				r32 l2 = det01p / det012;

					DrawPixel(buffer, x, y, (l0 * color0 + l1 * color1 + l2 * color2));
				}
            }
        }
    }
}