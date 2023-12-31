#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem
{
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
	const std::vector<std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
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
		textures_path("Tile_40.png"),  // platform
		textures_path("Tile_04.png"),  // step left section
		textures_path("Tile_02.png"),  // step middle section
		textures_path("Tiles_74.png"), // wall
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
		textures_path("indoor_0.png"),
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
		textures_path("locations/label_busloop.png"),
		textures_path("locations/label_beach.png"),
		textures_path("locations/label_library.png"),
		textures_path("locations/label_forest.png"),
 	  	textures_path("locations/label_street.png"),
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
		textures_path("busloop/bar-background.png"),
		textures_path("busloop/bus.png"),
	    textures_path("TBC.png"),
	    textures_path("darkLevel/dark_background0.png"),
		textures_path("darkLevel/dark_background1.jpg"),
		textures_path("darkLevel/light.png"),
		textures_path("darkLevel/collectible1.png"),
		textures_path("darkLevel/collectible2.png"),
		textures_path("darkLevel/collectible3.png"),
		textures_path("darkLevel/collectible4.png"),
		textures_path("darkLevel/collectible5.png"),
		textures_path("darkLevel/collectible6.png"),
		textures_path("forest/background/background_1.png"),
		textures_path("forest/background/background_2.png"),
		textures_path("forest/background/background_3.png"),
		textures_path("forest/background/background_4.png"),
		textures_path("forest/background/background_5.png"),
		textures_path("forest/interactable/tree.png"),
		textures_path("forest/interactable/box.png"),
		textures_path("forest/interactable/platform.png"),
		textures_path("forest/interactable/ladder.png"),
		textures_path("forest/items/tomato.png"),
		textures_path("forest/items/cherry.png"),
		textures_path("forest/items/meat.png"),
		textures_path("forest/items/radish.png"),
		textures_path("forest/items/squash.png"),
		textures_path("forest/items/strawberry.png"),
		textures_path("forest/items/mushroom.png"),
		textures_path("level0/door_sprite_sheet.png"),
		textures_path("beach/door_sprite_sheet.png"),
		textures_path("nest_door_sprite_sheet.png"),
		textures_path("mm/mm-background.png"),
		textures_path("mm/mm-plat.png"),
		textures_path("mm/mm-boss.png"),
		textures_path("mm/mm-hp-bar.png"),
		textures_path("mm/mm-hp.png"),
		textures_path("beach/beach_zombie.png"),
		textures_path("beach/npc_sprite_sheet.png"),
		textures_path("nest_student.png"),
		textures_path("cleaver_weapon.png"),
		textures_path("wallet-weapon.png"),
		textures_path("beach/beach_weapon.png"),
		textures_path("mm/mm-fountain.png"),
		textures_path("mm/mm-rain.png"),
		textures_path("mm/mm-door-sprite-sheet.png"),
		textures_path("mm/mm-projectile.png"),
		textures_path("locations/label_mm.png"),
		textures_path("tutorial/bus_bg.png"),
	  	textures_path("tutorial/bus_windows.png"),
	  	textures_path("locations/label_bus.png"),
		textures_path("locations/label_sewer.png"),
		textures_path("lab/map.png"),
		textures_path("lab/background-0.png"),
		textures_path("lab/2.png"),
		textures_path("lab/3.png"),
		textures_path("lab/4.png"),
		textures_path("lab/5.png"),
		textures_path("lab/6.png"),
		textures_path("lab/purple-pipes.png"),
		textures_path("lab/grey-pipes.png"),
		textures_path("lab/lab-decor.png"),
		textures_path("lab/lab-objects.png"),
		textures_path("lab/colour-overlay.png"),
		textures_path("lab/lab-ladder.png"),
		textures_path("lab/lab-npc.png"),
		textures_path("lab/lab-weapon.png"),
		textures_path("lab/lab-collect0.png"),
		textures_path("lab/lab-collect1.png"),
		textures_path("lab/lab-collect2.png"),
		textures_path("lab/lab-collect3.png"),
		textures_path("lab/lab-collect4.png"),
		textures_path("lab/lab-collect5.png"),
		textures_path("lab/boss-blockade.png"),
		textures_path("lab/lab-boss-sheet.png"),
		textures_path("cutscenes/cut_1.png"),
		textures_path("cutscenes/cut_2.png"),
		textures_path("cutscenes/cut_3.png"),
		textures_path("cutscenes/cut_4.png"),
        textures_path("menu/play.png"),
        textures_path("menu/menu-button.png"),
        textures_path("menu/back-button.png"),
        textures_path("menu/retry-button.png"),
        textures_path("ubz-title.png"),
        textures_path("menu/street.png"),
        textures_path("menu/lab.png"),
        textures_path("menu/busloop.png"),
        textures_path("menu/sewer.png"),
        textures_path("menu/wreck.png"),
        textures_path("menu/nest.png"),
        textures_path("menu/mainmall.png"),
        textures_path("menu/ikb.png"),
        textures_path("menu/bus.png"),
        textures_path("menu/forest.png"),
		textures_path("locations/label_lab.png"),
		textures_path("egg/nest-background.png"),
		textures_path("egg/nest-decor.png"),
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("spike"),
		shader_path("wheel"),
		shader_path("textured"),
		shader_path("overlay"),
		shader_path("blended"),
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

    void drawMenu(float elapsed_time_ms);

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
