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
const float WALL_WIDTH = 20.f;

const vec2 CLIMBABLE_DIM = { 30.f, 32.f };

const float ZOMBIE_SPEED = 150.f;
const float PLAYER_SPEED = 230;

// the player
Entity createBozo(RenderSystem* renderer, vec2 pos);
// the pointer
Entity createBozoPointer(RenderSystem* renderer, vec2 pos);
// the prey
Entity createStudent(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId);
// the enemy
Entity createZombie(RenderSystem* renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// one platform
Entity createPlatform(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture, bool visible, vec2 scale);
// helper for multiple platforms lined up
std::vector<Entity> createPlatforms(RenderSystem* renderer, float left_position_x, float left_position_y, uint num_tiles, TEXTURE_ASSET_ID texture, bool visible, vec2 scale);
// helper for steps
std::vector<Entity> createSteps(RenderSystem* renderer, vec2 left_pos, vec2 scale, uint num_steps, uint step_blocks, bool left);
// walls
Entity createWall(RenderSystem* renderer, float position_x, float position_y, float height, bool visible);
// ladders
std::vector<Entity> createClimbable(RenderSystem* renderer, float top_position_x, float top_position_y, uint num_sections, TEXTURE_ASSET_ID texture);
// spikes
Entity createSpike(RenderSystem* renderer, vec2 pos);
// wheels
Entity createWheel(RenderSystem* renderer, vec2 pos);
// background
Entity createBackground(RenderSystem* renderer, TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::PARALLAX_BACKGROUND_0, float depth = 0.f, vec2 position = { window_width_px / 2, window_height_px / 2 }, vec2 scale = { window_width_px, window_height_px });
// book
Entity createBook(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId);
// text box
Entity createStaticTexture(RenderSystem* renderer, TEXTURE_ASSET_ID textureID, vec2 position, std::string text, vec2 scale = { 1.f, 1.f });
// collectible
Entity createCollectible(RenderSystem* renderer, float position_x, float position_y, TEXTURE_ASSET_ID collectible, vec2 scale, bool overlay);

// hearts
Entity createHeart(RenderSystem* renderer, vec2 position, vec2 scale);

Entity createDangerous(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, vec2 p0, vec2 p1, vec2 p2, vec2 p3, bool cubic);

//label
Entity createLabel(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID textureId);

Entity createDoor(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID textureId);

void removeEntity(Entity e);

Entity createLoadingScreen(RenderSystem* renderer, vec2 position, vec2 scale);

// ----------------- Level variables go here -----------------
// Index 0 is level 1, index 1 is level 2 etc.

enum level {
	TUTORIAL = 0,
	NEST = 3,
	BEACH = 2,
	LIBRARY = 1,
  TBC = 4
};

// For swapping levels around
const std::vector<int> asset_mapping = {
  0, 
  3, 
  2, 
  1,
  4
};

const std::vector<std::string> LEVEL_DESCRIPTORS = {
  level_path("0_tutorial.json"),
  level_path("3_library.json"),
  level_path("2_beach.json"),
  level_path("1_nest.json"),
  level_path("to_be_continued.json"),
};

const std::string SAVE_STATE_FILE = level_path("save_state.json");

// ---------------------BACKGROUNDS-------------------------
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
		//{ TEXTURE_ASSET_ID::PARALLAX_FOREGROUND_0, 1.7f},
		//{ TEXTURE_ASSET_ID::PARALLAX_FOREGROUND_1, 1.5f},
		//{ TEXTURE_ASSET_ID::PARALLAX_FOREGROUND_3, 1.3f},
		//{ TEXTURE_ASSET_ID::PARALLAX_FOREGROUND_4, 1.1f},
		{ TEXTURE_ASSET_ID::BACKGROUND_INDOOR, 0.f},
		{ TEXTURE_ASSET_ID::BASEMENT, 0.f},
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
		{ TEXTURE_ASSET_ID::TBC, 0.f }
	}
};

/*
const std::vector<std::vector<TEXTURE_ASSET_ID>> BACKGROUND_ASSET = {
	{ TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND1, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND2, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND3, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND4, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND0 },
	{ TEXTURE_ASSET_ID::BACKGROUND, TEXTURE_ASSET_ID::BACKGROUND_INDOOR, TEXTURE_ASSET_ID::BASEMENT},
	{ TEXTURE_ASSET_ID::BEACH_SKY, TEXTURE_ASSET_ID::BEACH_SEA, TEXTURE_ASSET_ID::BEACH_LAND, TEXTURE_ASSET_ID::BEACH_CLOUD},
	{ TEXTURE_ASSET_ID::LIBRARY_FILL, TEXTURE_ASSET_ID::LIBRARY_OBJECTS, TEXTURE_ASSET_ID::LIBRARY_FRAME}
};
*/

const std::vector<TEXTURE_ASSET_ID> PLATFORM_ASSET = {
  TEXTURE_ASSET_ID::TUTORIAL_PLAT,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::BEACH_PLAT,
  TEXTURE_ASSET_ID::LIBRARY_PLAT,
};

const std::vector<TEXTURE_ASSET_ID> CLIMBABLE_ASSET = {
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::BEACH_LADDER,
  TEXTURE_ASSET_ID::LIBRARY_LAD,
};

const std::vector<TEXTURE_ASSET_ID> NPC_ASSET = {
  TEXTURE_ASSET_ID::TUTORIAL_NPC,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT
};

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
		TEXTURE_ASSET_ID::LIB_COLL2},
  {}
};

const std::vector<TEXTURE_ASSET_ID> WEAPON_ASSETS = {
	TEXTURE_ASSET_ID::TUTORIAL_WEAPON,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
};

// ---------------------SOUNDS-------------------------
const std::vector<std::string> BACKGROUND_MUSIC = {
	"tutorial.wav",
	"soundtrack.wav",
	"beach.wav",
	"library.wav"
};

const std::vector<TEXTURE_ASSET_ID> LABEL_ASSETS = {
	TEXTURE_ASSET_ID::LABEL_TUTORIAL,
	TEXTURE_ASSET_ID::LABEL_NEST,
	TEXTURE_ASSET_ID::LABEL_BEACH,
	TEXTURE_ASSET_ID::LABEL_LIB,
  TEXTURE_ASSET_ID::LABEL_LIB,
};
