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
	Camera playerCamera = Camera(0.f, 0.f, screen_width_px, screen_height_px);

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SPIKE, mesh_path("spike.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::WHEEL, mesh_path("wheel.obj"))
		// specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
		textures_path("loadingScreen.png"),
		textures_path("pause.png"),
		textures_path("student_sprite_sheet.png"),
		textures_path("zombie_sprite_sheet.png"),
		textures_path("bozo_sprite_sheet.png"),
		textures_path("bozo_pointer.png"),
		textures_path("Tile_40.png"),	// platform
		textures_path("Tile_04.png"),	// step left section
		textures_path("Tile_02.png"),	// step middle section
		textures_path("Tiles_74.png"),	// wall
		textures_path("egg.png"),
		textures_path("indoor-v3.png"),
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
		textures_path("win_screen.png"),
		textures_path("basement.png"),
		textures_path("indoor_4.png"),
		textures_path("indoor_3.png"),
		textures_path("indoor_1.png"),
		textures_path("indoor_0.png"),
		textures_path("background_0.png"),
		textures_path("background_1.png"),
		textures_path("background_2.png"),
		textures_path("background_3.png"),
		textures_path("beach/beach-plat.png"),
		textures_path("beach/beach-ladder.png"),
		textures_path("beach/spikeball.png"),
		textures_path("beach/cannon_blue.png"),
		textures_path("beach/beach-sky.png"),
		textures_path("beach/beach-sea.png"),
		textures_path("beach/beach-land.png"),
		textures_path("beach/beach-cloud.png"),
		textures_path("level0/tutorial_plat.png"),
		textures_path("level0/tutorial_foreground.png"),
		textures_path("level0/tutorial_background1.png"),
		textures_path("level0/tutorial_background2.png"),
		textures_path("level0/tutorial_background3.png"),
		textures_path("level0/tutorial_background4.png"),
		textures_path("level0/tutorial_move.png"),
		textures_path("level0/tutorial_climb.png"),
		textures_path("level0/tutorial_npcs.png"),
		textures_path("level0/tutorial_weapons.png"),
		textures_path("level0/tutorial_goal.png"),
		textures_path("level0/tutorial_npc_sprite_sheet.png"),
		textures_path("level0/weapon.png"),
		textures_path("level0/collectible1.png"),
		textures_path("level0/collectible2.png"),
		textures_path("level0/collectible3.png"),
		textures_path("library/frame.png"),
		textures_path("library/objects.png"),
		textures_path("library/fill.jpg"),
		textures_path("library/plat.png"),
		textures_path("library/ladder.png"),
		textures_path("locations/label_nest.png"),
		textures_path("locations/label_beach.png"),
		textures_path("locations/label_library.png"),
		textures_path("locations/label_tutorial.png"),
    textures_path("beach/beach-apple.png"),
    textures_path("beach/beach-chest.png"),
    textures_path("beach/beach-chest2.png"),
    textures_path("beach/beach-diamond.png"),
    textures_path("beach/beach-star.png"),
    textures_path("beach/beach-coin_01.png"),
    textures_path("door_sprite_sheet.png"),
    textures_path("beach/beach-bird.png"),
    textures_path("library/coll1.png"),
    textures_path("library/coll2.png"),
    textures_path("library/coll3.png"),
    textures_path("library/coll4.png"),
    textures_path("library/coll5.png"),
    textures_path("TBC.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("spike"),
		shader_path("wheel"),
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

	mat3 createProjectionMatrix(float left, float top, float right, float bottom);

	mat3 RenderSystem::createBasicProjectionMatrix();

	void resetCamera(vec2 defaultPos);

	vec4 getCameraBounds();

	void resetSpriteSheetTracker();

	static void deleteBufferId(int index);

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();
	void updateCameraBounds(float elapsed_time_ms);
	vec4 clampCam(float left, float top);
	int findFirstAvailableBufferSlot();

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
