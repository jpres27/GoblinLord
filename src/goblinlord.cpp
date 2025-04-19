internal void GameUpdateAndRender(Game_Offscreen_Buffer *buffer, Game_Controller_Input *input, v2 dd_player) 
{
	ClearScreen(buffer);

	v3 vertices[] =
	{
		V3(0.0f, 0.0f, 0.0f),
		V3(100.0f, 0.0f, 0.0f),
		V3(0.0f, 100.0f, 0.0f)
	};

	RGBA32 colors[] =
	{
    	CreateRGBA32(1.0f, 0.0f, 0.0f, 1.0f),
    	CreateRGBA32(0.0f, 1.0f, 0.0f, 1.0f),
    	CreateRGBA32(0.0f, 0.0f, 1.0f, 1.0f)
	};

	for(i32 i = 0; i < 100; ++i)
	{
		Mesh test_mesh = {};
		test_mesh.vertices = vertices;
		test_mesh.num_vertices = 3;
		test_mesh.colors = colors;

		DrawCommand command = {};
		command.mesh = test_mesh;
		command.cull_mode = none;
		command.transform =  {{
            1.f, 0.f, 0.f, input->mouse_x + 100.f * (i % 10),
            0.f, 1.f, 0.f, input->mouse_y + 100.f * (i / 10),
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
		}};

		Draw(buffer, &command, input);
	}
}
