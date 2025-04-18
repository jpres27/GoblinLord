internal void GameUpdateAndRender(Game_Offscreen_Buffer *buffer, Game_Controller_Input *input, v2 dd_player) 
{
	ClearScreen(buffer);

	v3 vertices[] =
	{
		V3(100.f, 100.f, 0.f),
		V3(200.f, 100.f, 0.f),
		V3(100.f, 200.f, 0.f),
	};

	RGBAReal32 test_color = {};
	test_color.red = 0.8f;
	test_color.green = 0.4f;
	test_color.blue = 0.0f;

	Mesh test_mesh = {};
	test_mesh.positions = vertices;
	test_mesh.num_vertices = 3;
	test_mesh.colorf = test_color;

	DrawCommand command = {};
	command.mesh = test_mesh;
	command.cull_mode = cw;

	Draw(buffer, &command);
}
