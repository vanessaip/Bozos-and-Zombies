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
const float STEP_HEIGHT = 30.f;
const float STEP_WIDTH = 30.f;
const vec2 CLIMBABLE_DIM = { 30.f, 32.f };

const float ZOMBIE_SPEED = 100.f;

// not constant since they can change from level to level
//static float PLATFORM_HEIGHT;
//static float PLATFORM_WIDTH;      // TODO (Justin): figure out how to set this per level

static int curr_level = 0;

const std::vector<vec2> PLATFORM_SCALES = {
	{ 32.f, 30.f },
	{ 50.f, 30.f },
	{ 50.f, 30.f },
	{ 48.f, 17.f }
};

static float PLATFORM_WIDTH = PLATFORM_SCALES[curr_level].x;
static float PLATFORM_HEIGHT = PLATFORM_SCALES[curr_level].y;

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
Entity createPlatform(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture, bool visible, vec2 scale = { PLATFORM_WIDTH, PLATFORM_HEIGHT });
// helper for multiple platforms lined up
std::vector<Entity> createPlatforms(RenderSystem* renderer, float left_position_x, float left_position_y, uint num_tiles, TEXTURE_ASSET_ID texture, bool visible, vec2 scale);
// helper for steps
std::vector<Entity> createSteps(RenderSystem* renderer, vec2 left_pos, uint num_steps, uint step_blocks, bool left);
// walls
Entity createWall(RenderSystem* renderer, float position_x, float position_y, float height, bool visible);
// ladders
std::vector<Entity> createClimbable(RenderSystem* renderer, float top_position_x, float top_position_y, uint num_sections, TEXTURE_ASSET_ID texture);
// spikes
Entity createSpike(RenderSystem* renderer, vec2 pos);
// background
Entity createBackground(RenderSystem* renderer, TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::BACKGROUND, vec2 position = { window_width_px / 2, window_height_px / 2 }, vec2 scale = { window_width_px, window_height_px });
// book
Entity createBook(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId);
// text box
Entity createStaticTexture(RenderSystem* renderer, TEXTURE_ASSET_ID textureID, vec2 position, std::string text, vec2 scale = { 1.f, 1.f });
// collectible
Entity createCollectible(RenderSystem* renderer, float position_x, float position_y, TEXTURE_ASSET_ID collectible, vec2 scale, bool overlay);

// hearts
Entity createHeart(RenderSystem* renderer, vec2 position, vec2 scale);

Entity createDangerous(RenderSystem* renderer, vec2 position, vec2 scale);

// ----------------- Level variables go here -----------------
// Index 0 is level 1, index 1 is level 2 etc.

// ---------------------BACKGROUNDS-------------------------
const std::vector<std::vector<TEXTURE_ASSET_ID>> BACKGROUND_ASSET = {
	{ TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND1, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND2, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND3, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND4, TEXTURE_ASSET_ID::TUTORIAL_BACKGROUND0 },
	{ TEXTURE_ASSET_ID::BACKGROUND, TEXTURE_ASSET_ID::BACKGROUND_INDOOR, TEXTURE_ASSET_ID::BASEMENT},
	{ TEXTURE_ASSET_ID::BEACH_SKY, TEXTURE_ASSET_ID::BEACH_SEA, TEXTURE_ASSET_ID::BEACH_LAND, TEXTURE_ASSET_ID::BEACH_CLOUD},
	{ TEXTURE_ASSET_ID::LIBRARY_FILL, TEXTURE_ASSET_ID::LIBRARY_OBJECTS, TEXTURE_ASSET_ID::LIBRARY_FRAME}
};

// ---------------------PLATFORMS-------------------------
const std::vector<std::vector<vec4>> PLATFORM_POSITIONS = {
   {
		// Ground
		{15.f, window_height_px - 20.f, 45, true},

		// Building 1
		{15.f, window_height_px - 315.f, 10, true},
		{79.f, window_height_px - 120.f, 3, true},
		{240.f, window_height_px - 200.f, 3, true},

		// Building 2
		{368.f, 235.f, 3, true},
		{463.f, 145.f, 2, true},
		{527.5f, window_height_px - 510.f, 1, true},
		{560.f, window_height_px - 405.f, 8, true},
		{432.f, window_height_px - 283.f, 4, true},
		{432.f, window_height_px - 147.f, 9, true},

		// Building 3
		{window_width_px - 50.f, window_height_px - 280.f, 1, true},
		{window_width_px - 80.f, window_height_px - 375.f, 1, true},
		{window_width_px - 110.f, window_height_px - 310.f, 1, true},
		{window_width_px - 142.f, window_height_px - 215.f, 1, true},
		{window_width_px - 174.f, window_height_px - 115.f, 1, true},
		{window_width_px - 240.f, window_height_px - 150.f, 2, true},
		{window_width_px - 404.f, window_height_px - 310.f, 5, true},
		{window_width_px - 496.f, window_height_px - 407.f, 3, true},
		{window_width_px - 496.f, window_height_px - 150.f, 2, true},
		{window_width_px - 560.f, window_height_px - 310.f, 2, true},
  },
  {
	{window_width_px / 2 - 50.f * 7.5, window_height_px - 12.f, 16, true},
	{50.f * 4, window_height_px * 0.8, 8, true},
	{window_width_px - 50.f * 6, window_height_px * 0.8, 2, true},
	{110.f, window_height_px * 0.6, 7, true},
	{window_width_px - 50.f * 7 - 80.f, window_height_px * 0.6, 7, true},
	{110.f, window_height_px * 0.4, 7, true},
	{window_width_px - 50.f * 10 - 80.f, window_height_px * 0.4, 10, true},
	{110.f, window_height_px * 0.2, 25, true}
  },
  {
	{0, window_height_px - 12.f, 30, true},
	{window_width_px - 300, window_height_px - 120.f, 10, true},
	{0, window_height_px - 200.f, 10, true},
	{0, 200.f, 6, true},
	{600, window_height_px - 320.f, 8, true},
	{200, window_height_px - 485.f, 11, true},
	{450, 110, 2, true},
	{window_width_px - 500, 110, 11, true},
	{window_width_px - 250, window_height_px / 2, 6, true},
  },
  {
	{45.f, window_height_px - 45.f, 30, false},
	{400.f, window_height_px - 160.f, 6, true},
	{75.f, window_height_px / 2 + 140.f, 6, true},
	{250.f, window_height_px / 2 - 75.f, 2, true},
	{600.f, 495.f, 3, true},
	{750.f, 400.f, 2, true},
	{1100.f, 545.f, 1, true},
	{1100.f, 195.f, 3, true},
	{45.f, 45.f, 30, false},
  }
};

const std::vector<TEXTURE_ASSET_ID> PLATFORM_ASSET = {
  TEXTURE_ASSET_ID::TUTORIAL_PLAT,
  TEXTURE_ASSET_ID::STEP1,
  TEXTURE_ASSET_ID::BEACH_PLAT,
  TEXTURE_ASSET_ID::LIBRARY_PLAT,
};


// Sorted from bottom of screen to top of screen
const std::vector<std::vector<float>> FLOOR_POSITIONS = {
	{
		window_height_px - 20.f,
		window_height_px - 310.f,
		window_height_px - 315.f,
		//window_height_px - 120.f,
		//window_height_px - 200.f,

		window_height_px / 2,
		window_height_px - 283.f,
		window_height_px - 147.f,

		//window_height_px - 150.f,
		//window_height_px - 407.f,
	},
	{
		window_height_px - 12.f,
		window_height_px * 0.8,
		window_height_px * 0.6,
		window_height_px * 0.4,
		window_height_px * 0.2 },
  { window_height_px - 12.f,
	window_height_px - 200.f,
	window_height_px - 485.f,
	110},
  {600, 500, 350, 250},
};


// ---------------------CLIMBABLES-------------------------
const std::vector<std::vector<vec3>> CLIMBABLE_POSITIONS = {
	{
		{20.f, window_height_px - (20.f + 9 * CLIMBABLE_DIM.y + 7.f), 9},
		{15.f + 13 * PLATFORM_WIDTH, 235.f, 9},
		{15.f + 13 * PLATFORM_WIDTH, window_height_px - (20.f + 4 * CLIMBABLE_DIM.y + 2.f), 4},
		{15.f + 24 * PLATFORM_WIDTH, window_height_px - (20.f + 12 * CLIMBABLE_DIM.y + 2.f), 12},
		{15.f + 27 * PLATFORM_WIDTH, window_height_px - (20.f + 9 * CLIMBABLE_DIM.y + 2.f), 9},
		{15.f + 36 * PLATFORM_WIDTH, window_height_px - (20.f + 9 * CLIMBABLE_DIM.y + 2.f), 5},
	},
	{
		{PLATFORM_SCALES[1].x * 9, window_height_px * 0.795, 5},
		{PLATFORM_SCALES[1].x * 7, window_height_px * 0.6, 5},
		{window_width_px - PLATFORM_SCALES[1].x * 6, window_height_px * 0.6, 5},
		{PLATFORM_SCALES[1].x * 4, window_height_px * 0.2, 10},
		{window_width_px - PLATFORM_SCALES[1].x * 4, window_height_px * 0.4, 5},
		{window_width_px - PLATFORM_SCALES[1].x * 9, window_height_px * 0.2, 5}
	},
	{
		{200, window_height_px - 485, 9},
		{400, window_height_px - 200, 6},
		{window_width_px - 500, 110, 12}
	},
	{
		{300, window_height_px / 2 - 70.f , 7},
		{600.f , 500.f, 5},
		{1100.f, 200.f, 11}
	}
};

const std::vector<TEXTURE_ASSET_ID> CLIMBABLE_ASSET = {
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::LADDER2,
  TEXTURE_ASSET_ID::BEACH_LADDER,
  TEXTURE_ASSET_ID::LIBRARY_LAD,
};

const std::vector<std::vector<std::vector<float>>> ZOMBIE_CLIMB_POINTS = {
	{
		{20.f},
		{15.f + 13 * PLATFORM_WIDTH},
		{15.f + 24 * PLATFORM_WIDTH},
		{15.f + 27 * PLATFORM_WIDTH},
		{15.f + 36 * PLATFORM_WIDTH}
	},
	{
		{PLATFORM_SCALES[1].x * 9},
		{PLATFORM_SCALES[1].x * 7, window_width_px - PLATFORM_SCALES[1].x * 6},
		{PLATFORM_SCALES[1].x * 4, window_width_px - PLATFORM_SCALES[1].x * 4},
		{PLATFORM_SCALES[1].x * 4, window_width_px - PLATFORM_SCALES[1].x * 9}
	},
	{
		{400},
		{200},
		{window_width_px - 500}
	},
	{}
};


// ---------------------WALLS-------------------------
const std::vector<std::vector<vec4>> WALL_POSITIONS = {
  {},
  {
	{ 320.f, window_height_px * 0.9 + 10.f, window_height_px * 0.2 - 10.f, true},
	{ window_width_px - 320.f, window_height_px * 0.9 + 10.f , window_height_px * 0.2 - 10.f, true},
	{180.f, window_height_px * 0.7 + 15.f , window_height_px * 0.2, true},
	{window_width_px - 220.f, window_height_px * 0.7 + 15.f , window_height_px * 0.2, true},
	{80.f, window_height_px * 0.4 - 20.f , window_height_px * 0.4 + 70.f, true},
	{window_width_px - 100.f, window_height_px * 0.4 - 20 , window_height_px * 0.4 + 70.f, true}
  },
  {},
  {
	{32.f, window_height_px / 2, window_height_px, false},
	{window_width_px - 32.f, window_height_px / 2, window_height_px, false},
  },
};


// ---------------------SPIKES-------------------------
const std::vector<std::vector<vec2>> SPIKE_POSITIONS = {
	{},
	{
		{ 260.f, 625.f },
		{ 50.f , window_height_px - 5.f },
		{ 150.f , window_height_px - 5.f },
		{ window_width_px - 50.f , window_height_px - 5.f },
		{ window_width_px - 150.f , window_height_px - 5.f }
	},
	{},
	{}
};


// ---------------------ZOMBIES-------------------------
const std::vector<std::vector<vec2>> ZOMBIE_START_POS = {
	{
		{ 25.f, window_height_px - 25.f }
	},
	{
		{ 1200, 600 }, {300, 440}, {200, 440}
	},
	{},
	{}
};

// -----------------------NPCs---------------------------
const std::vector<std::vector<vec2>> NPC_START_POS = {
	{
	   {570.f, window_height_px - 405.f - STUDENT_BB_HEIGHT / 2.f},
	},
	{
		{ 1000, 440 }, { 300, 440 }, { 900, 280 }, {400, window_height_px * 0.4 - 50.f}, {600, window_height_px * 0.2 - 50.f}
	},
	{},
	{}
};

const std::vector<TEXTURE_ASSET_ID> NPC_ASSET = {
  TEXTURE_ASSET_ID::TUTORIAL_NPC,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT,
  TEXTURE_ASSET_ID::STUDENT
};

// ---------------------BOZO-------------------------
const std::vector<vec2> BOZO_STARTING_POS = {
  {window_width_px - 200, window_height_px - 50.f},
  {500, window_height_px * 0.8 - 50.f },
  {window_width_px / 2, window_height_px - 50},
  {100, window_height_px - 100.f} };

const std::vector<vec2> ENEMY_SPAWN_POS = {
	{},
	{  window_width_px - 100.f, 120.f },
	{},
	{}
};



// ---------------------COLLECTIBLES-------------------------
const std::vector<std::vector<vec2>> COLLECTIBLE_POSITIONS = {
	{
		{window_width_px - 80.f, window_height_px - 398.f},
		{480.f, 121.f},
		{50.f, window_height_px - 338.f}
	},
	{
		{1310, 136},
		{102, 298},
		{660, 134},
		{860, 296},
		{1200, 622},
		{350, 770}
	},
	{},
	{},
	{}
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
	{},
	{}
};

const std::vector<vec2> COLLECTIBLE_SCALES =
{
	{ 20, 20 },
	{ 30, 30 },
	{ 30, 30 },
	{ 30, 30 }
};

const std::vector<TEXTURE_ASSET_ID> WEAPON_ASSETS = {
	TEXTURE_ASSET_ID::TUTORIAL_WEAPON,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
	TEXTURE_ASSET_ID::BOOK,
};

// ---------------------SOUNDS-------------------------
const std::vector<std::string> BACKGROUND_MUSIC = {
	"soundtrack.wav",
	"soundtrack.wav",
	"beach.wav",
	"library.wav"
};
