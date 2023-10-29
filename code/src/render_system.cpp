// internal
#include "render_system.hpp"
#include <SDL.h>
#include <iostream>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);
	transform.reflect(motion.reflect);

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	
	GLuint vbo; 
	GLuint ibo;
	if (render_request.used_geometry == GEOMETRY_BUFFER_ID::SPRITE_SHEET)
	{
		assert(registry.spriteSheets.has(entity));
		const SpriteSheet& spriteSheet = registry.spriteSheets.get(entity);
		vbo = spriteSheet.bufferId + 1;
		ibo = vertex_buffers.size() + vbo;
	}
	else 
	{
		vbo = vertex_buffers[(GLuint)render_request.used_geometry];
		ibo = index_buffers[(GLuint)render_request.used_geometry];
	}


	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)render_request.used_texture];
		ivec2 texture_dim = texture_dimensions[(GLuint)render_request.used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	/*
	// FUTURE: won't need for now, could reuse if we end up having meshes
	else if (render_request.used_effect == EFFECT_ASSET_ID::SALMON)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();

		if (render_request.used_effect == EFFECT_ASSET_ID::SALMON)
		{
			// Light up?
			GLint light_up_uloc = glGetUniformLocation(program, "light_up");
			assert(light_up_uloc >= 0);

			// !!! TODO A1: set the light_up shader variable using glUniform1i,
			// similar to the glUniform1f call below. The 1f or 1i specified the type, here a single int.
			gl_has_errors();
		}
	}
	*/
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WATER]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::WATER];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(water_program, "screen_darken_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.screen_darken_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw(float elapsed_time_ms)
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 1, 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix(elapsed_time_ms);
	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

void RenderSystem::step(float elapsed_time_ms) {
	for (int i = 0; i < registry.spriteSheets.components.size(); i++) {
		SpriteSheet& sheet = registry.spriteSheets.components[i];
		sheet.timer_ms += elapsed_time_ms;

		if (sheet.timer_ms > sheet.switchTime_ms) {
			sheet.offset.x += sheet.spriteDim.x;
			sheet.timer_ms = 0;

			if (sheet.offset.x / sheet.spriteDim.x >= sheet.getCurrentSpriteCount())
				sheet.offset.x = 0.f;

			updateSpriteSheetGeometryBugger(sheet);
		}
	}
}

mat3 RenderSystem::createProjectionMatrix(float elapsed_time_ms)
{
	// Set projection matrix to define camera bounds
	float left = camera.left;
	float top = camera.top;
	float right = camera.right; 
	float bottom = camera.bottom;

	Motion& playerMotion = registry.motions.get(registry.players.entities[0]);

	// handle x-position of camera
	if (playerMotion.velocity.x > 0) {
		camera.xOffset = 100.f;
		if (!lastPlayerDirectionIsPos)
		{
			lastPlayerDirectionIsPos = true;
			camera.shiftHorizontal = true;
			camera.timer_ms_x = 0;
		}
	}
	else if (playerMotion.velocity.x < 0) {
		camera.xOffset = -100.f;
		if (lastPlayerDirectionIsPos)
		{
			lastPlayerDirectionIsPos = false;
			camera.shiftHorizontal = true;
			camera.timer_ms_x = 0;
		}
	}
	else
		lastRestingPlayerPos = playerMotion.position;
		
	if (camera.shiftHorizontal && abs(lastRestingPlayerPos.x - playerMotion.position.x) < 32.f) 
	{
		// Don't adjust camera if little steps are taken to allow small position adjustments without disorienting the user
	}
	else 
	{
		// inerpolate camera "position" to get smooth movement
		float nextLeft = (playerMotion.position.x + playerMotion.velocity.x * 2.f * elapsed_time_ms / 1000.f - (screen_width_px / 2.0)) + camera.xOffset;
		left = left + (nextLeft - left) * (camera.timer_ms_x / camera.timer_stop_ms);

		camera.timer_ms_x += elapsed_time_ms;
		camera.shiftHorizontal = false;
	}

	// handle y-position changes
	float nextTop = (playerMotion.position.y - (screen_height_px / 2.0));
	float verticalDiff = abs(nextTop - top);

	if (verticalDiff > (screen_height_px / 3.f - 60.f))  // this comparison depends on how we set up the level (may need to adjust)
		camera.shiftVertical = true;

	if (camera.shiftVertical)
	{
		top = top + (nextTop - top) * (camera.timer_ms_y / camera.timer_stop_ms); // interpolate for smooth movement
		camera.timer_ms_y += 2.f * elapsed_time_ms;
		
		if (camera.timer_ms_y / camera.timer_stop_ms >= 1 || verticalDiff < 1)  // takes too long if we wait for it to be exactly 0
		{
			top = nextTop;
			camera.shiftVertical = false;
			camera.timer_ms_y = 0.f;
		}
	}

	// bound camera to level boundaries
	left = max<float>(left, 0);
	right = min<float>(left + screen_width_px, window_width_px * 1.f);
	if (right == window_width_px * 1.f)
		left = right - screen_width_px;
	
	top = max(top, 0.f);
	bottom = min<float>(top + screen_height_px, window_height_px * 1.f);
	if (bottom == window_height_px * 1.f)
		top = bottom - screen_height_px;

	camera.left = left;
	camera.top = top;
	camera.right = right;
	camera.bottom = bottom;

	gl_has_errors();

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

void RenderSystem::resetCamera() 
{
	camera.left = 0.f;
	camera.top = 0.f;
	camera.right = screen_width_px;
	camera.bottom = screen_height_px;
}

void RenderSystem::resetSpriteSheetTracker() {
	RenderSystem::spriteSheetBuffersCount = geometry_count;
}

template <class T>
void RenderSystem::bindSpriteSheetVBO(uint gid, std::vector<T> vertices) {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	gl_has_errors();
}

template <class T>
void RenderSystem::bindSpriteSheetVBOandIBO(uint gid, std::vector<T> vertices, std::vector<uint16_t> indices) {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[gid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
	gl_has_errors();
}

void RenderSystem::initializeSpriteSheet(Entity& entity, ANIMATION_MODE defaultMode, std::vector<int> spriteCounts, float switchTime, vec2 trunc) {
	GLuint id = RenderSystem::spriteSheetBuffersCount + 1;
	RenderSystem::spriteSheetBuffersCount++;

	SpriteSheet& sheet = registry.spriteSheets.emplace(entity, SpriteSheet(id, defaultMode, spriteCounts, switchTime, trunc));

	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };

	textured_vertices[0].texcoord = { sheet.offset.x, sheet.offset.y + sheet.spriteDim.y };
	textured_vertices[1].texcoord = { sheet.offset.x + sheet.spriteDim.x - sheet.truncation.x, sheet.offset.y + sheet.spriteDim.y };
	textured_vertices[2].texcoord = { sheet.offset.x + sheet.spriteDim.x - sheet.truncation.x, sheet.offset.y + sheet.truncation.y };
	textured_vertices[3].texcoord = { sheet.offset.x, sheet.offset.y + sheet.truncation.y };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindSpriteSheetVBOandIBO(sheet.bufferId, textured_vertices, textured_indices);
}

void RenderSystem::updateSpriteSheetGeometryBugger(SpriteSheet& sheet) {
	assert(sheet.spriteDim.x > 0);
	assert(sheet.spriteDim.y > 0);

	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };

	textured_vertices[0].texcoord = { sheet.offset.x, sheet.offset.y + sheet.spriteDim.y };
	textured_vertices[1].texcoord = { sheet.offset.x + sheet.spriteDim.x - sheet.truncation.x, sheet.offset.y + sheet.spriteDim.y };
	textured_vertices[2].texcoord = { sheet.offset.x + sheet.spriteDim.x - sheet.truncation.x, sheet.offset.y + sheet.truncation.y };
	textured_vertices[3].texcoord = { sheet.offset.x, sheet.offset.y + sheet.truncation.y };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindSpriteSheetVBOandIBO(sheet.bufferId, textured_vertices, textured_indices);
}