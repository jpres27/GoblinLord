internal void GameUpdateAndRender(Game_Offscreen_Buffer *buffer, Game_Controller_Input *input, v2 dd_player) 
{
	ClearScreen(buffer);

	v3 vertices[] =
	{
		V3(0.0f, 0.0f, 0.0f),
		V3(100.0f, 0.0f, 0.0f),
		V3(0.0f, 100.0f, 0.0f)
	};

	for(i32 i = 0; i < 100; ++i)
	{
		RGBAReal32 test_color = {};
		test_color.red = (i%3) == 0;
		test_color.green = (i%3) == 1;
		test_color.blue = (i%3) == 2;
		test_color.alpha = 1.0f;

		Mesh test_mesh = {};
		test_mesh.positions = vertices;
		test_mesh.num_vertices = 3;
		test_mesh.colorf = test_color;

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
