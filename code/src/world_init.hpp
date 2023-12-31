#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <string.h>

// These are ahrd coded to the dimensions of the entity texture
const float STUDENT_BB_WIDTH = 30.f;
const float STUDENT_BB_HEIGHT = 50.f;
const float ZOMBIE_BB_WIDTH = 50.f;
const float ZOMBIE_BB_HEIGHT = STUDENT_BB_HEIGHT + 20.f;
const float BOZO_BB_WIDTH = STUDENT_BB_WIDTH;
const float BOZO_BB_HEIGHT = STUDENT_BB_HEIGHT;
const float BOZO_POINTER_BB_WIDTH = 170.f;
const float BOZO_POINTER_BB_HEIGHT = 170.f;

const vec2 CLIMBABLE_DIM = { 30.f, 32.f };

const float ZOMBIE_SPEED = 120.f;
const float PLAYER_SPEED = 230;
const float MMBOSS_SPEED = 140.f;
const float BOSS_KNOCKBACK = 300.f;

// the player
Entity createBozo(RenderSystem* renderer, vec2 pos);
// the pointer
Entity createBozoPointer(RenderSystem* renderer, vec2 pos);
// the prey
Entity createStudent(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId);
// the enemy
Entity createZombie(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// one platform
Entity createPlatform(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture, bool visible, vec2 scale);
// helper for multiple platforms lined up
std::vector<Entity> createPlatforms(RenderSystem* renderer, float left_position_x, float left_position_y, uint num_tiles, TEXTURE_ASSET_ID texture, bool visible, vec2 scale);
// helper for steps
std::vector<Entity> createSteps(RenderSystem* renderer, vec2 left_pos, vec2 scale, uint num_steps, uint step_blocks, bool left);
// walls
Entity createWall(RenderSystem* renderer, float position_x, float position_y, float width, float height, bool visible, TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::WALL);
// ladders
std::vector<Entity> createClimbable(RenderSystem* renderer, float top_position_x, float top_position_y, uint num_sections, TEXTURE_ASSET_ID texture);
// spikes
Entity createSpike(RenderSystem* renderer, vec2 pos);
// wheels
Entity createWheel(RenderSystem* renderer, vec2 pos);
// background
Entity createBackground(RenderSystem* renderer, TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::PARALLAX_BACKGROUND_0, float depth = 0.f, vec2 position = { window_width_px / 2, window_height_px / 2 }, bool blended = false, vec2 scale = { window_width_px, window_height_px });
// book
Entity createBook(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId);
// text box
Entity createStaticTexture(RenderSystem* renderer, TEXTURE_ASSET_ID textureID, vec2 position, std::string text, vec2 scale = { 1.f, 1.f });
// collectible
Entity createCollectible(RenderSystem* renderer, float position_x, float position_y, TEXTURE_ASSET_ID collectible, vec2 scale, bool overlay, bool isPoisonous);
// hearts
Entity createHeart(RenderSystem* renderer, vec2 position, vec2 scale);

// Entity createDangerous(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, vec2 p0, vec2 p1, vec2 p2, vec2 p3, bool cubic);

Entity createBus(RenderSystem* renderer, vec2 position, vec2 scale, vec2 velocity = { 0.f, 0.f });

// dangerous object
Entity createDangerous(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, vec2 p0, vec2 p1, vec2 p2, vec2 p3, bool cubic, bool bezier, int spriteCount);
// label
Entity createOverlay(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID textureId, bool is_fading);
// door
Entity createDoor(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID textureId);
// loading screen
Entity createLoadingScreen(RenderSystem* renderer, vec2 position, vec2 scale);
// lights
Entity createLight(RenderSystem* renderer, vec2 position, float intensity_dropoff_factor);

void removeEntity(Entity e);

Entity createLoadingScreen(RenderSystem* renderer, vec2 position, vec2 scale);

Entity createBoss(RenderSystem* renderer, vec2 position, vec2 scale, float health, float damage, TEXTURE_ASSET_ID assetID, vec2 trunc, std::vector<int> counts);

Entity createHPBar(RenderSystem* renderer, vec2 position);

Entity createHP(RenderSystem* renderer, vec2 position);

Entity createAnimatedBackgroundObject(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, std::vector<int> spriteCounts, vec2 trunc);

Entity createCutscene(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, std::vector<int> spriteCounts, float switchTime, vec2 trunc);

// ----------------- Level variables go here -----------------
// Index 0 is level 1, index 1 is level 2 etc.

enum level {
	CUT_1 = 0,
	BUS = 1,
	CUT_2 = 2,
	BUSLOOP = 3,
	TUTORIAL = 4,
	LIBRARY = 5,
	CUT_3 = 6,
	MMBOSS = 7,
	NEST = 8,
	BEACH = 9,
	SEWERS = 10,
	FOREST = 11,
	LAB = 12,
	CUT_4 = 13
};

// For swapping levels around
const std::vector<int> asset_mapping = {
  6,
  5,
  10,
  13,
  0,
  3,
  11,
  4,
  1,
  2,
  7,
  8,
  9,
  12
};

const std::vector<std::string> LEVEL_DESCRIPTORS = {
  level_path("7_cut1.json"),
  level_path("5_bus.json"),
  level_path("8_cut2.json"),
  level_path("4_busloop.json"),
  level_path("0_tutorial.json"),
  level_path("3_library.json"),
  level_path("9_cut3.json"),
  level_path("4_mmboss.json"),
  level_path("1_nest.json"),
  level_path("2_beach.json"),
  level_path("5_sewers.json"),
  level_path("5_forest.json"),
  level_path("6_lab.json"),
  level_path("10_cut4.json")
};

const std::string SAVE_STATE_FILE = level_path("save_state.json");

// ---------------------BACKGROUNDS-------------------------
// uses asset_mapping
const std::vector<std::vector<std::tuple<TEXTURE_ASSET_ID, float>>> BACKGROUND_ASSET = {
	{
		{ TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND1, 16.f },
		{ TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND2, 8.f },
		{ TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND3, 4.f },
		{ TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND4, 2.f },
		{ TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND0, 0.f }
	},
	{
		// texture id, depth (set depth to 0 if not scrolling)
		{ TEXTURE_ASSET_ID::PARALLAX_BACKGROUND_0, 8.0f },
		{ TEXTURE_ASSET_ID::PARALLAX_BACKGROUND_1, 6.0f},
		{ TEXTURE_ASSET_ID::PARALLAX_BACKGROUND_2, 4.0f},
		{ TEXTURE_ASSET_ID::PARALLAX_BACKGROUND_3, 2.0f},
		{ TEXTURE_ASSET_ID::NEST_ROOF, 0.f},
		{ TEXTURE_ASSET_ID::BASEMENT, 0.f},
		{ TEXTURE_ASSET_ID::NEST_DECOR, 0.f}
	},
	{
		{ TEXTURE_ASSET_ID::BEACH_SKY, 16.0f},
		{ TEXTURE_ASSET_ID::BEACH_SEA, 8.0f},
		{ TEXTURE_ASSET_ID::BEACH_LAND, 2.0f},
		{ TEXTURE_ASSET_ID::BEACH_CLOUD, 4.0f}
	},
	{
		{ TEXTURE_ASSET_ID::LIBRARY_FILL, 4.f },
		{ TEXTURE_ASSET_ID::LIBRARY_OBJECTS, 2.f },
		{ TEXTURE_ASSET_ID::LIBRARY_FRAME, 0.f }
	},
	{
		{ TEXTURE_ASSET_ID::MM_BACKGROUND, 0.f }
	},
	{
	  {TEXTURE_ASSET_ID::BUS_BG, 2.f},
	  {TEXTURE_ASSET_ID::BUS_WINDOW, 0.f}
	},
	{},
	{
		{ TEXTURE_ASSET_ID::DARK_BACKGROUND1, 2.f },
		{ TEXTURE_ASSET_ID::DARK_BACKGROUND0, 0.f }
	},
	{
		{ TEXTURE_ASSET_ID::FOREST_BACKGROUND_1, 32.f },
		{ TEXTURE_ASSET_ID::FOREST_BACKGROUND_2, 16.f },
		{ TEXTURE_ASSET_ID::FOREST_BACKGROUND_3, 8.f },
		{ TEXTURE_ASSET_ID::FOREST_BACKGROUND_4, 4.f },
		{ TEXTURE_ASSET_ID::FOREST_BACKGROUND_5, 0.f }
	},
	{
		{ TEXTURE_ASSET_ID::LAB_BACKGROUND_0, 0.f},
		// { TEXTURE_ASSET_ID::LAB_BACKGROUND_1, 16.f},
		{ TEXTURE_ASSET_ID::LAB_BACKGROUND_2, 12.f},
		{ TEXTURE_ASSET_ID::LAB_BACKGROUND_3, 10.f},
		{ TEXTURE_ASSET_ID::LAB_BACKGROUND_4, 10.f},
		{ TEXTURE_ASSET_ID::LAB_BACKGROUND_5, 6.f},
		{ TEXTURE_ASSET_ID::LAB_BACKGROUND_6, 4.f},
		{ TEXTURE_ASSET_ID::LAB_BACKGROUND_7, 2.f},
		{ TEXTURE_ASSET_ID::LAB_DECOR_0, 0.f},
		{ TEXTURE_ASSET_ID::LAB_DECOR_1, 0.f},
		{ TEXTURE_ASSET_ID::LAB_OVERLAY, 0.f},
		{ TEXTURE_ASSET_ID::LAB_MAP, 0.f},
	},
	{},
	{},
	{},
	{
		{ TEXTURE_ASSET_ID::BUSLOOP_BACKGROUND, 0.f }
	},
};
// uses asset_mapping
const std::vector<TEXTURE_ASSET_ID> PLATFORM_ASSET = {
  TEXTURE_ASSET_ID::TUTORIAL_PLAT,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::BEACH_PLAT,
  TEXTURE_ASSET_ID::LIBRARY_PLAT,
  TEXTURE_ASSET_ID::MM_PLAT,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::TUTORIAL_PLAT,
  TEXTURE_ASSET_ID::FOREST_PLATFORM,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::STEP1
};
// uses asset_mapping
const std::vector<TEXTURE_ASSET_ID> CLIMBABLE_ASSET = {
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::BEACH_LADDER,
  TEXTURE_ASSET_ID::LIBRARY_LAD,
  TEXTURE_ASSET_ID::LIBRARY_LAD,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::FOREST_LADDER,
  TEXTURE_ASSET_ID::LAB_LADDER,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::BEACH_LADDER
};
// does NOT use asset_mapping
const std::vector<TEXTURE_ASSET_ID> DOOR_ASSET = {
	TEXTURE_ASSET_ID::GHETTO_DOOR,
	TEXTURE_ASSET_ID::NEST_DOOR,
	TEXTURE_ASSET_ID::BEACH_DOOR,
	TEXTURE_ASSET_ID::LIBRARY_DOOR,
	TEXTURE_ASSET_ID::MM_DOOR,
	TEXTURE_ASSET_ID::NEST_DOOR,
	TEXTURE_ASSET_ID::NEST_DOOR,
	TEXTURE_ASSET_ID::GHETTO_DOOR,
	TEXTURE_ASSET_ID::BEACH_DOOR,
	TEXTURE_ASSET_ID::NEST_DOOR,
	TEXTURE_ASSET_ID::NEST_DOOR,
	TEXTURE_ASSET_ID::NEST_DOOR,
	TEXTURE_ASSET_ID::NEST_DOOR,
	TEXTURE_ASSET_ID::BEACH_DOOR
};
// does NOT use asset_mapping
const std::vector<TEXTURE_ASSET_ID> NPC_ASSET = {
  TEXTURE_ASSET_ID::TUTORIAL_NPC,
  TEXTURE_ASSET_ID::NEST_NPC,
  TEXTURE_ASSET_ID::BEACH_NPC,
  TEXTURE_ASSET_ID::NEST_NPC,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::LAB_NPC,
	TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
};

// uses asset_mapping
const std::vector<TEXTURE_ASSET_ID> ZOMBIE_ASSET = {
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::BEACH_ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
	TEXTURE_ASSET_ID::ZOMBIE,
};

// uses asset mapping
const std::vector<std::vector<TEXTURE_ASSET_ID>> COLLECTIBLE_ASSETS = {
	{
		TEXTURE_ASSET_ID::TUTORIAL_COLLECTIBLE1,
		TEXTURE_ASSET_ID::TUTORIAL_COLLECTIBLE2,
		TEXTURE_ASSET_ID::TUTORIAL_COLLECTIBLE3,
	},
	{
		TEXTURE_ASSET_ID::BURGER,
		TEXTURE_ASSET_ID::MUFFIN,
		TEXTURE_ASSET_ID::SODA,
		TEXTURE_ASSET_ID::NOODLES,
		TEXTURE_ASSET_ID::ONIGIRI,
		TEXTURE_ASSET_ID::PIZZA
	},
	{
		TEXTURE_ASSET_ID::BEACH_APPLE,
		TEXTURE_ASSET_ID::BEACH_CHEST,
		TEXTURE_ASSET_ID::BEACH_CHEST2,
		TEXTURE_ASSET_ID::BEACH_DIAMOND,
		TEXTURE_ASSET_ID::BEACH_STAR,
		TEXTURE_ASSET_ID::BEACH_COIN
	},
	{
		TEXTURE_ASSET_ID::LIB_COLL1,
		TEXTURE_ASSET_ID::LIB_COLL5,
		TEXTURE_ASSET_ID::LIB_COLL3,
		TEXTURE_ASSET_ID::LIB_COLL4,
		TEXTURE_ASSET_ID::LIB_COLL2
	},
	{},
	{
		TEXTURE_ASSET_ID::BURGER
	},
  {},
	{
		TEXTURE_ASSET_ID::SEWER_COLLECT2,
		TEXTURE_ASSET_ID::SEWER_COLLECT3,
		TEXTURE_ASSET_ID::SEWER_COLLECT4,
		TEXTURE_ASSET_ID::SEWER_COLLECT6,
	},
	{
		TEXTURE_ASSET_ID::FOREST_CHERRY,
		TEXTURE_ASSET_ID::FOREST_MEAT,
		TEXTURE_ASSET_ID::FOREST_RADISH,
		TEXTURE_ASSET_ID::FOREST_SQUASH,
		TEXTURE_ASSET_ID::FOREST_TOMATO,
		TEXTURE_ASSET_ID::FOREST_STRAWBERRY,
		TEXTURE_ASSET_ID::FOREST_MUSHROOM
	},
	{
		TEXTURE_ASSET_ID::LAB_COLLECT0,
		TEXTURE_ASSET_ID::LAB_COLLECT1,
		TEXTURE_ASSET_ID::LAB_COLLECT2,
		TEXTURE_ASSET_ID::LAB_COLLECT3,
		TEXTURE_ASSET_ID::LAB_COLLECT4,
		TEXTURE_ASSET_ID::LAB_COLLECT5,
	},
  {},
  {},
  {},
	{
		TEXTURE_ASSET_ID::BEACH_COIN,
		TEXTURE_ASSET_ID::BEACH_APPLE,
		TEXTURE_ASSET_ID::MUFFIN,
	},
};
// uses asset_mapping
const std::vector<TEXTURE_ASSET_ID> WEAPON_ASSETS = {
	TEXTURE_ASSET_ID::TUTORIAL_WEAPON,
	TEXTURE_ASSET_ID::NEST_WEAPON,
	TEXTURE_ASSET_ID::BEACH_WEAPON,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::MM_PROJECTILE,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::NEST_WEAPON,
	TEXTURE_ASSET_ID::LAB_WEAPON,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK
};
// uses asset_mapping
const std::vector<TEXTURE_ASSET_ID> LABEL_ASSETS = {
	TEXTURE_ASSET_ID::LABEL_STREET,
	TEXTURE_ASSET_ID::LABEL_NEST,
	TEXTURE_ASSET_ID::LABEL_BEACH,
	TEXTURE_ASSET_ID::LABEL_LIB,
  TEXTURE_ASSET_ID::LABEL_MM,
  TEXTURE_ASSET_ID::LABEL_BUS,
  TEXTURE_ASSET_ID::LABEL_BUS,
  TEXTURE_ASSET_ID::LABEL_SEWER,
  TEXTURE_ASSET_ID::LABEL_FOREST,
  TEXTURE_ASSET_ID::LABEL_LAB,
  TEXTURE_ASSET_ID::LABEL_BUS,
  TEXTURE_ASSET_ID::LABEL_BUS,
  TEXTURE_ASSET_ID::LABEL_BUS,
	TEXTURE_ASSET_ID::LABEL_BUSLOOP
};


// ---------------------BOSSES-------------------------
const std::vector<TEXTURE_ASSET_ID> BOSS_ASSET = {
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::MM_BOSS,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::LAB_BOSS,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
};

const std::vector<TEXTURE_ASSET_ID> CUTSCENE_ASSET = {
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_1,
  TEXTURE_ASSET_ID::CUT_2,
  TEXTURE_ASSET_ID::CUT_3,
  TEXTURE_ASSET_ID::CUT_4,
  TEXTURE_ASSET_ID::CUT_1
};