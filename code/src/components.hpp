#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include <cassert>
#include "../ext/stb_image/stb_image.h"
#include <chrono>

// Player component
struct Player
{
	// order is left, right, up, down
	vec4 keyPresses = { false, false, false, false };
};

struct PlayerEffects
{

};

// Zombies can turn Humans into Zombies
struct Zombie
{
  bool right_side_collision = false;
  bool left_side_collision = false;
};

// Player and Student(s) are Human
struct Human
{
};

struct Platform
{
};

struct Wall
{
};

struct Spike
{

};

struct Wheel
{

};

// Ladders and maybe stairs?
struct Climbable
{

};

struct Book
{
	bool offHand = true;
};

struct Door
{
	float animation_switch_time = 200.f; // milliseconds
	int door_open_frames = 6; // maybe can be specified in level
	float door_open_timer = door_open_frames * animation_switch_time;
};

struct Bounce
{
	float mass;
};

struct Poisonous
{
};

// All data relevant to the shape and motion of entities
struct Motion
{
	vec2 position;
	float angle;
	vec2 velocity;
	vec2 scale;
	// First boolean is reflection on x axis with true for reflected
	// Second boolean is reflection on y axis with true for reflected
	vec2 reflect;
	bool offGround;
	bool climbing;
	float speedMultiplier;
	Motion(vec2 position = { 0.f, 0.f }, float angle = 0.f, vec2 velocity = { 0.f, 0.f }, vec2 scale = { 10.f, 10.f }, vec2 reflect = { false, false }, bool offGround = true, bool climbing = false, float speedMultiplier = 1.0)
	{
		this->position = position;
		this->angle = angle;
		this->velocity = velocity;
		this->scale = scale;
		this->reflect = reflect;
		this->offGround = offGround;
		this->climbing = climbing;
		this->speedMultiplier = speedMultiplier;
	}
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other_entity; // the second object involved in the collision
	//	int collision_type; // 0 is interactive collision, 1 is collision for stand on platform, 2 is for bounce back collision
	Collision(Entity& other_entity)
	{
		this->other_entity = other_entity;
		//		this->collision_type = collision_type;
	};
};

// Data structure for toggling debug mode
struct Debug
{
	bool in_full_view_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float screen_darken_factor = -1;
	float is_poisoned = false;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying player, direction is 0 when rotationg left, direction is 1 when rotating right
struct DeathTimer
{
	float timer_ms = 3000.f;
	bool direction = 0;
};

// A timer that will be associated to a NPC player that converts into a Zombie
struct InfectTimer
{
	float timer_ms = 3000.f;
	bool direction = 0;
};

// When the player has lost a life, it should be invincible for a few seconds
struct LostLife
{
	float timer_ms = 3000.f;
	bool direction = 0;
};

struct ZombieDeathTimer
{
	float timer_ms = 600.f;
};

// Keyframe animation stores all keyframes and timing data for a given entity
struct KeyframeAnimation
{
	int num_of_frames = 0; // total number of keyframes
	int curr_frame = 0;
	float timer_ms = 0.f;	 // time since keyframe was updated
	float switch_time = 0.f; // time when next keyframe should be loaded in
	bool loop = false;
	std::vector<Motion> motion_frames; // keyframes

	KeyframeAnimation(int num_of_frames, float switch_time, bool loop, std::vector<Motion>& frames)
	{
		this->num_of_frames = num_of_frames;
		this->switch_time = switch_time;
		this->loop = loop;
		this->motion_frames = frames;
	}
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1, 1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct Camera
{
	float left;
	float top;
	float right;
	float bottom;
	float timer_ms_x = 0.f;
	float timer_ms_y = 0.f;
	float timer_stop_ms = 9000.f;
	float xOffset = 0.f;
	float yOffset = 0.f;
	bool shiftHorizontal = false;
	bool shiftVertical = false;

	Camera(float l, float t, float r, float b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
};

struct Background
{
	float depth = 0.f;
	Camera parallaxCam = Camera(0.f, 0.f, 0.f, 0.f);
};

struct TextBox
{
	std::string text = "";
};

struct Collectible
{
	int collectible_id;
};

struct Overlay
{

};

struct Dangerous
{
	vec2 p0;
	vec2 p1;
	vec2 p2;
	vec2 p3;
	bool cubic;
	float bezier_time = 0;
	bool bezier;
};

struct Fading
{
	float fading_factor = 1.f;
	std::chrono::time_point<std::chrono::steady_clock> fading_timer;
};

struct Boss
{
	float health = 100.f;

	// num hearts that the boss damages
	float damage = 1.f;

	float summon_timer_ms = 10000.f;
	bool rain_active = false;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID
{
	// define parallax scrolling backgrounds at start
	LOADING_SCREEN = 0,
	PAUSE = LOADING_SCREEN + 1,
	STUDENT = PAUSE + 1,
	ZOMBIE = STUDENT + 1,
	BOZO = ZOMBIE + 1,
	BOZO_POINTER = BOZO + 1,
	PLATFORM = BOZO_POINTER + 1,
	STEP0 = PLATFORM + 1,
	STEP1 = STEP0 + 1,
	WALL = STEP1 + 1,
	EGG0 = WALL + 1,
	BACKGROUND_INDOOR = EGG0 + 1,
	LADDER1 = BACKGROUND_INDOOR + 1,
	LADDER2 = LADDER1 + 1,
	LADDER3 = LADDER2 + 1,
	BOOK = LADDER3 + 1,
	TUTORIAL1 = BOOK + 1,
	BURGER = TUTORIAL1 + 1,
	MUFFIN = BURGER + 1,
	NOODLES = MUFFIN + 1,
	ONIGIRI = NOODLES + 1,
	PIZZA = ONIGIRI + 1,
	SODA = PIZZA + 1,
	HEART = SODA + 1,
	WIN_SCREEN = HEART + 1,
	BASEMENT = WIN_SCREEN + 1,
	PARALLAX_FOREGROUND_4 = BASEMENT + 1,
	PARALLAX_FOREGROUND_3 = PARALLAX_FOREGROUND_4 + 1,
	PARALLAX_FOREGROUND_1 = PARALLAX_FOREGROUND_3 + 1,
	PARALLAX_FOREGROUND_0 = PARALLAX_FOREGROUND_1 + 1,
	PARALLAX_BACKGROUND_3 = PARALLAX_FOREGROUND_0 + 1,
	PARALLAX_BACKGROUND_2 = PARALLAX_BACKGROUND_3 + 1,
	PARALLAX_BACKGROUND_1 = PARALLAX_BACKGROUND_2 + 1,
	PARALLAX_BACKGROUND_0 = PARALLAX_BACKGROUND_1 + 1,
	BEACH_PLAT = PARALLAX_BACKGROUND_0 + 1,
	BEACH_LADDER = BEACH_PLAT + 1,
	SPIKE_BALL = BEACH_LADDER + 1,
	CANNON = SPIKE_BALL + 1,
	BEACH_SKY = CANNON + 1,
	BEACH_SEA = BEACH_SKY + 1,
	BEACH_LAND = BEACH_SEA + 1,
	BEACH_CLOUD = BEACH_LAND + 1,
	TUTORIAL_PLAT = BEACH_CLOUD + 1,
	TUTORIAL_BACKGROUND0 = TUTORIAL_PLAT + 1,
	TUTORIAL_BACKGROUND1 = TUTORIAL_BACKGROUND0 + 1,
	TUTORIAL_BACKGROUND2 = TUTORIAL_BACKGROUND1 + 1,
	TUTORIAL_BACKGROUND3 = TUTORIAL_BACKGROUND2 + 1,
	TUTORIAL_BACKGROUND4 = TUTORIAL_BACKGROUND3 + 1,
	TUTORIAL_MOVEMENT = TUTORIAL_BACKGROUND4 + 1,
	TUTORIAL_CLIMB = TUTORIAL_MOVEMENT + 1,
	TUTORIAL_NPCS = TUTORIAL_CLIMB + 1,
	TUTORIAL_WEAPONS = TUTORIAL_NPCS + 1,
	TUTORIAL_GOAL = TUTORIAL_WEAPONS + 1,
	TUTORIAL_NPC = TUTORIAL_GOAL + 1,
	TUTORIAL_WEAPON = TUTORIAL_NPC + 1,
	TUTORIAL_COLLECTIBLE1 = TUTORIAL_WEAPON + 1,
	TUTORIAL_COLLECTIBLE2 = TUTORIAL_COLLECTIBLE1 + 1,
	TUTORIAL_COLLECTIBLE3 = TUTORIAL_COLLECTIBLE2 + 1,
	LIBRARY_FRAME = TUTORIAL_COLLECTIBLE3 + 1,
	LIBRARY_OBJECTS = LIBRARY_FRAME + 1,
	LIBRARY_FILL = LIBRARY_OBJECTS + 1,
	LIBRARY_PLAT = LIBRARY_FILL + 1,
	LIBRARY_LAD = LIBRARY_PLAT + 1,
	LABEL_NEST = LIBRARY_LAD + 1,
	LABEL_BEACH = LABEL_NEST + 1,
	LABEL_LIB = LABEL_BEACH + 1,
	LABEL_TUTORIAL = LABEL_LIB + 1,
	LABEL_FOREST = LABEL_TUTORIAL + 1,
  LABEL_STREET = LABEL_FOREST + 1,
	BEACH_APPLE = LABEL_FOREST + 1,
	BEACH_CHEST = BEACH_APPLE + 1,
	BEACH_CHEST2 = BEACH_CHEST + 1,
	BEACH_DIAMOND = BEACH_CHEST2 + 1,
	BEACH_STAR = BEACH_DIAMOND + 1,
	BEACH_COIN = BEACH_STAR + 1,
	LIBRARY_DOOR = BEACH_COIN + 1,
	BEACH_BIRD = LIBRARY_DOOR + 1,
	LIB_COLL1 = BEACH_BIRD + 1,
	LIB_COLL2 = LIB_COLL1 + 1,
	LIB_COLL3 = LIB_COLL2 + 1,
	LIB_COLL4 = LIB_COLL3 + 1,
	LIB_COLL5 = LIB_COLL4 + 1,
  	TBC = LIB_COLL5 + 1,
	FOREST_BACKGROUND_1 = TBC + 1,
	FOREST_BACKGROUND_2 = FOREST_BACKGROUND_1 + 1,
	FOREST_BACKGROUND_3 = FOREST_BACKGROUND_2 + 1,
	FOREST_BACKGROUND_4 = FOREST_BACKGROUND_3 + 1,
	FOREST_BACKGROUND_5 = FOREST_BACKGROUND_4 + 1,
	FOREST_TREE = FOREST_BACKGROUND_5 + 1,
	FOREST_BOX = FOREST_TREE + 1,
	FOREST_PLATFORM = FOREST_BOX + 1,
	FOREST_LADDER = FOREST_PLATFORM + 1,
	FOREST_TOMATO = FOREST_LADDER + 1,
	FOREST_CHERRY = FOREST_TOMATO + 1,
	FOREST_MEAT = FOREST_CHERRY + 1,
	FOREST_RADISH = FOREST_MEAT + 1,
	FOREST_SQUASH = FOREST_RADISH + 1,
	FOREST_STRAWBERRY = FOREST_SQUASH + 1,
	FOREST_MUSHROOM = FOREST_STRAWBERRY + 1,
	GHETTO_DOOR = FOREST_MUSHROOM + 1,
	BEACH_DOOR = GHETTO_DOOR + 1,
	NEST_DOOR = BEACH_DOOR + 1,
	MM_BACKGROUND = NEST_DOOR + 1,
	MM_PLAT = MM_BACKGROUND + 1,
	MM_BOSS = MM_PLAT + 1,
	HP_BAR = MM_BOSS + 1,
	HP = HP_BAR + 1,
	BEACH_ZOMBIE = HP + 1,
	BEACH_NPC = BEACH_ZOMBIE + 1,
	NEST_NPC = BEACH_NPC + 1,
	CLEAVER_WEAPON = NEST_NPC + 1,
	BEACH_WEAPON = CLEAVER_WEAPON + 1,
	MM_FOUNTAIN = BEACH_WEAPON + 1,
	MM_RAIN = MM_FOUNTAIN + 1,
	MM_DOOR = MM_RAIN + 1,
	MM_PROJECTILE = MM_DOOR + 1,
	LABEL_MM = MM_PROJECTILE + 1,
	BUS_BG = LABEL_MM + 1,
	BUS_WINDOW = BUS_BG + 1,
	LABEL_BUS = BUS_WINDOW + 1,
	TEXTURE_COUNT = LABEL_BUS + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
	COLOURED = 0,
	SPIKE = COLOURED + 1, // can reuse if we end up having meshes
	WHEEL = SPIKE + 1,
	TEXTURED = WHEEL + 1,
	WATER = TEXTURED + 1,
	EFFECT_COUNT = WATER + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPIKE = 0,
	WHEEL = SPIKE + 1,
	SPRITE = WHEEL + 1,
	DEBUG_LINE = SPRITE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	SPRITE_SHEET = SCREEN_TRIANGLE + 1,
	GEOMETRY_COUNT = SPRITE_SHEET + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest
{
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

enum class ANIMATION_MODE
{
	IDLE = 0,
	RUN = IDLE + 1,
	ATTACK = RUN + 1,
	HURT = ATTACK + 1,
	CLIMB = HURT + 1,
	FIFTH_INDEX = CLIMB + 1,
	SIXTH_INDEX = FIFTH_INDEX + 1,
	SEVENTH_INDEX = SIXTH_INDEX + 1,
	EIGTH_INDEX = SEVENTH_INDEX + 1,
	NINTH_INDEX = EIGTH_INDEX + 1,
	MODE_COUNT = NINTH_INDEX + 1
};
const int animation_mode_count = (int)ANIMATION_MODE::MODE_COUNT;

struct SpriteSheet
{
	float timer_ms = 0.f;
	float switchTime_ms;
	float numberOfModes;
	vec2 offset = { 0.f, 0.f };
	vec2 spriteDim = { -1.f, -1.f };
	vec2 truncation;
	std::vector<int> spriteCount;
	uint bufferId;
	ANIMATION_MODE mode = ANIMATION_MODE::IDLE;

	SpriteSheet(uint bId, ANIMATION_MODE defaultMode, std::vector<int>& spriteCt, float switchTime, vec2 trunc)
	{
		bufferId = bId;
		spriteCount = spriteCt;
		switchTime_ms = switchTime;
		truncation = trunc;
		numberOfModes = spriteCt.size();

		double maxCount = *std::max_element(spriteCount.begin(), spriteCount.end());
		spriteDim.x = float(1.f / maxCount);
		spriteDim.y = float(1.f / numberOfModes);

		updateAnimation(defaultMode);
	}

	void updateAnimation(ANIMATION_MODE newMode) {
		mode = newMode;
		if ((int)mode >= 0 && (int)mode < numberOfModes) {
			offset.y = ((int)mode) * spriteDim.y;
		}
		else
			offset.y = 0.f;
	}

	int getCurrentSpriteCount() {
		if ((int)mode >= 0)
			return spriteCount[(int)mode];
		else
			return 0;
	}
};