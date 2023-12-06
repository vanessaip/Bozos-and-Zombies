// internal
#include "render_system.hpp"
#include <SDL.h>
#include <iostream>

#include "tiny_ecs_registry.hpp"

float screen_width = screen_width_px;
float screen_height = screen_width_px;

int bufferIds[50];

float previousLeft = 0.f;
float currentLeft = 0.f;

//glm::vec3 lights[2] = { { 250, 175, 1.1f }, { 1300 , 700, 1.5f } };

void RenderSystem::drawTexturedMesh(Entity entity,
	const mat3& projection)
{
	Motion& motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);
	transform.reflect(motion.reflect);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

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
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)render_request.used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Lighting
		GLuint hasLights_loc = glGetUniformLocation(program, "hasLights");
		GLuint lights_loc = glGetUniformLocation(program, "lights");
		if (registry.lights.size() > 0) 
		{
			glUniform1f(hasLights_loc, true);
			glUniform3fv(lights_loc, registry.lights.components.size(), reinterpret_cast<GLfloat*>(&registry.lights.components[0]));
		}
		else 
		{
			glUniform1f(hasLights_loc, false);
			vec3 lights[2] = { {0,0,0}, {0,0,0} };
			glUniform3fv(lights_loc, sizeof(lights) / sizeof(glm::vec3), reinterpret_cast<GLfloat*>(&lights[0]));
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}

	else if (render_request.used_effect == EFFECT_ASSET_ID::BLENDED) 
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)render_request.used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE);
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::OVERLAY_TEXTURED) 
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)render_request.used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Fading
		GLuint fade_timer_uloc = glGetUniformLocation(program, "fading_factor");
		if (registry.fading.has(entity)) {
			Fading& fade = registry.fading.get(entity);
			glUniform1f(fade_timer_uloc, fade.fading_factor);
		}
		else {
			glUniform1f(fade_timer_uloc, 0.f);
		}
	}

	// FUTURE: won't need for now, could reuse if we end up having meshes
	else if (render_request.used_effect == EFFECT_ASSET_ID::SPIKE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();

		if (render_request.used_effect == EFFECT_ASSET_ID::SPIKE)
		{
			// Light up?
			GLint light_up_uloc = glGetUniformLocation(program, "light_up");
			assert(light_up_uloc >= 0);

			// !!! TODO A1: set the light_up shader variable using glUniform1i,
			// similar to the glUniform1f call below. The 1f or 1i specified the type, here a single int.
			gl_has_errors();
		}
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::WHEEL)
	{
		GLint wheelColor_uloc = glGetUniformLocation(program, "wheelColor");
		GLint spikeColor_uloc = glGetUniformLocation(program, "spikeColor");
		gl_has_errors();

		// Define colors for the wheel and spikes
		vec3 wheelColor = vec3(0.5f, 0.35f, 0.05); // Brown color for the wheel
		vec3 spikeColor = vec3(0.628, 0.095, 0.990); // Purple color for the spikes

		// Set the uniform values for the colors
		glUniform3fv(wheelColor_uloc, 1, (float*)&wheelColor);
		glUniform3fv(spikeColor_uloc, 1, (float*)&spikeColor);
		gl_has_errors();

		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);
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
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
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
	GLuint is_poisoned = glGetUniformLocation(water_program, "is_poisoned");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState& screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.screen_darken_factor);
	glUniform1f(is_poisoned, screen.is_poisoned);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
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
	glViewport(0, 0, window_width_px, window_height_px);
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

	float prevLeft = playerCamera.left;
	updateCameraBounds(elapsed_time_ms);
	float currLeft = playerCamera.left;

	mat3 projection_2D = createProjectionMatrix(playerCamera.left, playerCamera.top, playerCamera.right, playerCamera.bottom);
	mat3 projectionBasic = createBasicProjectionMatrix();
	mat3 projectionParallax;

	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		bool isParallax = false;
		if (registry.backgrounds.has(entity))
		{
			Background& background = registry.backgrounds.get(entity);
			// adjust projection matrix based on depth of scrolling background
			if (background.depth > 0)
			{
				isParallax = true;
				float horizontalShift = (currLeft - prevLeft) / background.depth; // horizontal shift inversely proportional to depth
				vec4 clampedBounds = clampCam(background.parallaxCam.left + horizontalShift, playerCamera.top);
				background.parallaxCam.left = clampedBounds[0];
				background.parallaxCam.top = clampedBounds[1];
				background.parallaxCam.right = clampedBounds[2];
				background.parallaxCam.bottom = clampedBounds[3];

				projectionParallax = createProjectionMatrix(
					background.parallaxCam.left,
					background.parallaxCam.top,
					background.parallaxCam.right,
					background.parallaxCam.bottom);
			}
		}

		if (registry.overlay.has(entity)) {
			drawTexturedMesh(entity, projectionBasic);
		}
		else {

			if (isParallax)
				drawTexturedMesh(entity, projectionParallax);
			else
				drawTexturedMesh(entity, projection_2D);

		}
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

void RenderSystem::drawMenu(float elapsed_time_ms) {
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
	mat3 projection_2D = createBasicProjectionMatrix();
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

			if (sheet.offset.x / sheet.spriteDim.x >= sheet.getCurrentSpriteCount()) {
				sheet.offset.x = 0.f;
			}

			updateSpriteSheetGeometryBuffer(sheet);
		}
	}
}

mat3 RenderSystem::createProjectionMatrix(float left, float top, float right, float bottom)
{
	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}

mat3 RenderSystem::createBasicProjectionMatrix() {
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float)window_width_px;
	float bottom = (float)window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}

void RenderSystem::resetCamera(vec2 defaultPos)
{
	/*
	camera.left = max<float>((defaultPos.x - (screen_width_px / 2.0)) + camera.xOffset, 0);
	camera.right = min<float>(camera.left + screen_width_px, window_width_px * 1.f);
	if (camera.right == window_width_px * 1.f)
		camera.left = camera.right - screen_width_px;

	camera.top = max(camera.top, 0.f);
	camera.bottom = min<float>(camera.top + screen_height_px, window_height_px * 1.f);
	if (camera.bottom == window_height_px * 1.f)
		camera.top = camera.bottom - screen_height_px;
	*/
	playerCamera.left = 0.f;
	playerCamera.top = 0.f;
	playerCamera.right = playerCamera.left + screen_width;
	playerCamera.bottom = playerCamera.top + screen_height;

	playerCamera.timer_ms_x = 0.f;
	playerCamera.timer_ms_y = 0.f;
}

vec4 RenderSystem::getCameraBounds() {
	return { playerCamera.left, playerCamera.top, playerCamera.right, playerCamera.bottom };
}

void RenderSystem::resetSpriteSheetTracker() {
	std::fill_n(bufferIds, sizeof(bufferIds) / sizeof(int), -1);

	for (int i = 0; i < geometry_count; i++)
	{
		bufferIds[i] = i;
	}
}

template <class T>
void RenderSystem::bindVBOandIBO(uint gid, std::vector<T> vertices, std::vector<uint16_t> indices) {
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
	GLuint id = findFirstAvailableBufferSlot();
	assert(id != -1);
	bufferIds[id] = id;

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
	bindVBOandIBO(sheet.bufferId, textured_vertices, textured_indices);
}

void RenderSystem::updateSpriteSheetGeometryBuffer(SpriteSheet& sheet) {
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
	bindVBOandIBO(sheet.bufferId, textured_vertices, textured_indices);
}

void RenderSystem::updateCameraBounds(float elapsed_time_ms)
{
	if (debugging.in_full_view_mode) {
		screen_width = window_width_px;
		screen_height = window_height_px;
	}
	else {
		screen_width = screen_width_px;
		screen_height = screen_height_px;
	}
	// Set projection matrix to define camera bounds
	float left = playerCamera.left;
	float top = playerCamera.top;
	float right = playerCamera.right;
	float bottom = playerCamera.bottom;

	Motion& playerMotion = registry.motions.get(registry.players.entities[0]);

	// handle x-position of camera
	if (playerMotion.velocity.x > 0) {
		playerCamera.xOffset = 100.f;
		if (!lastPlayerDirectionIsPos)
		{
			lastPlayerDirectionIsPos = true;
			playerCamera.shiftHorizontal = true;
			playerCamera.timer_ms_x = 0;
		}
	}
	else if (playerMotion.velocity.x < 0) {
		playerCamera.xOffset = -100.f;
		if (lastPlayerDirectionIsPos)
		{
			lastPlayerDirectionIsPos = false;
			playerCamera.shiftHorizontal = true;
			playerCamera.timer_ms_x = 0;
		}
	}
	else
		lastRestingPlayerPos = playerMotion.position;

	if (playerCamera.shiftHorizontal && abs(lastRestingPlayerPos.x - playerMotion.position.x) < 32.f)
	{
		// Don't adjust camera if little steps are taken to allow small position adjustments without disorienting the user
	}
	else
	{
		// inerpolate camera "position" to get smooth movement
		float nextLeft = (playerMotion.position.x + playerMotion.velocity.x * 2.f * elapsed_time_ms / 1000.f - (screen_width / 2.0)) + playerCamera.xOffset;


		if (playerCamera.timer_ms_x / playerCamera.timer_stop_ms < 1.f)
			left = left + (nextLeft - left) * (playerCamera.timer_ms_x / playerCamera.timer_stop_ms);
		else
		{
			left = nextLeft;
			playerCamera.timer_ms_x = 0.f;
		}

		playerCamera.timer_ms_x += elapsed_time_ms;
		playerCamera.shiftHorizontal = false;
	}

	// handle y-position changes
	float nextTop = (playerMotion.position.y - (screen_height / 2.0));
	float verticalDiff = abs(nextTop - top);

	if (verticalDiff > (screen_height / 3.f - 60.f))  // this comparison depends on how we set up the level (may need to adjust)
		playerCamera.shiftVertical = true;

	if (playerCamera.shiftVertical)
	{
		top = top + (nextTop - top) * (playerCamera.timer_ms_y / playerCamera.timer_stop_ms); // interpolate for smooth movement
		playerCamera.timer_ms_y += 2.f * elapsed_time_ms;

		if (playerCamera.timer_ms_y / playerCamera.timer_stop_ms >= 1 || verticalDiff < 1)  // takes too long if we wait for it to be exactly 0
		{
			top = nextTop;
			playerCamera.shiftVertical = false;
			playerCamera.timer_ms_y = 0.f;
		}
	}

	// bound camera to level boundaries
	vec4 clampedBounds = clampCam(left, top);

	playerCamera.left = clampedBounds[0];
	playerCamera.top = clampedBounds[1];
	playerCamera.right = clampedBounds[2];
	playerCamera.bottom = clampedBounds[3];
}

vec4 RenderSystem::clampCam(float left, float top)
{
	left = max<float>(left, 0);
	float right = min<float>(left + screen_width, window_width_px * 1.f);
	if (right == window_width_px * 1.f)
		left = right - screen_width;

	top = max(top, 0.f);
	float bottom = min<float>(top + screen_height, window_height_px * 1.f);
	if (bottom == window_height_px * 1.f)
		top = bottom - screen_height;

	return { left, top, right, bottom };
}

int RenderSystem::findFirstAvailableBufferSlot()
{
	int size = sizeof(bufferIds) / sizeof(int);
	for (int i = geometry_count; i < size; i++)
	{
		if (bufferIds[i] == -1)
			return i;
	}
	return -1;
}

void RenderSystem::deleteBufferId(int index) {
	assert(index >= geometry_count && index < sizeof(bufferIds) / sizeof(int));
	bufferIds[index] = -1;
}