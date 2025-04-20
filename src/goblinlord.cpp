internal void GameUpdateAndRender(Game_Offscreen_Buffer *buffer, Game_Controller_Input *input, v2 dd_player) 
{
	ClearScreen(buffer);

	Viewport viewport = {};
	viewport.x_min = 0;
	viewport.y_min = 0;
	viewport.x_max = (i32)buffer->width;
	viewport.y_max = (i32)buffer->height;

	v3 vertices[] =
	{
		V3(0.0f, 0.5f, 0.0f),
		V3(-0.5f, -0.5f, 0.0f),
		V3(0.5f, -0.5f, 0.0f),
	};

	RGBA32 colors[] =
	{
    	CreateRGBA32(1.0f, 0.0f, 0.0f, 1.0f),
    	CreateRGBA32(0.0f, 1.0f, 0.0f, 1.0f),
    	CreateRGBA32(0.0f, 0.0f, 1.0f, 1.0f)
	};

	Mesh test_mesh = {};
	test_mesh.vertices = vertices;
	test_mesh.num_vertices = 3;
	test_mesh.colors = colors;

	DrawCommand command = {};
	command.mesh = test_mesh;
	command.cull_mode = none;
	command.transform = IdentityMatrix();

	Draw(buffer, &viewport, &command, input);
}
