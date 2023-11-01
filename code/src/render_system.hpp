#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	vec2 lastRestingPlayerPos;
	bool lastPlayerDirectionIsPos = true; // true = +x, false = -x
	Camera camera = Camera(0.f, 0.f, screen_width_px, screen_height_px);

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SPIKE, mesh_path("spike.obj"))
		// specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
		textures_path("student_sprite_sheet.png"),
		textures_path("zombie_sprite_sheet.png"),
		textures_path("bozo_sprite_sheet.png"),
		textures_path("bozo_pointer.png"),
		textures_path("business-background.png"),
		textures_path("Tile_40.png"),	// platform
		textures_path("Tile_04.png"),	// step left section
		textures_path("Tile_02.png"),	// step middle section
		textures_path("Tiles_74.png"),	// wall
		textures_path("egg.png"),
		textures_path("2-2.png"),
		textures_path("Ladder1.png"),
		textures_path("Ladder2.png"),
		textures_path("Ladder3.png"),
		textures_path("book.png"),
		textures_path("tutorial1.png"),
		textures_path("burger.png"),
		textures_path("muffin.png"),
		textures_path("noodles.png"),
		textures_path("onigiri.png"),
		textures_path("pizza.png"),
		textures_path("soda.png"),
		textures_path("heart.png"),
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("spike"),
		shader_path("textured"),
		shader_path("water") };

	// TODO (Justin): update size of array if we exceed 50 sprite sheet entities 
	std::array<GLuint, 50> vertex_buffers;
	std::array<GLuint, 50> index_buffers;
	std::array<Mesh, geometry_count> meshes;

	// vertex and index buffers for sprite sheets
	std::vector<GLuint> sprite_vertex_buffers;
	std::vector<GLuint> sprite_index_buffers;

public:
	uint spriteSheetBuffersCount = 0;
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	template <class T>
	void bindVBOandIBO(uint gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();

	void updateSpriteSheetGeometryBuffer(SpriteSheet& sheet);

	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw(float elapsed_time_ms);

	void step(float elapsed_time_ms);

	void initializeSpriteSheet(Entity& entity, ANIMATION_MODE defaultMode, std::vector<int> spriteCounts, float switchTime, vec2 trunc);

	mat3 createProjectionMatrix(float elapsed_time_ms);

	mat3 RenderSystem::createBasicProjectionMatrix();

	void resetCamera(vec2 defaultPos);

	vec4 getCameraBounds();

	void resetSpriteSheetTracker();

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
