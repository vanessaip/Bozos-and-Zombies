#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include <cassert>
#include "../ext/stb_image/stb_image.h"

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
};

// Player and Student(s) are Human
struct Human
{
};

struct Background
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

// Ladders and maybe stairs?
struct Climbable
{

};

struct Book
{
	bool offHand = true;
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
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float screen_darken_factor = -1;
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

struct TextBox
{
	std::string text = "";
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
	STUDENT = 0,
	ZOMBIE = STUDENT + 1,
	BOZO = ZOMBIE + 1,
	BOZO_POINTER = BOZO + 1,
	BACKGROUND = BOZO_POINTER + 1,
	PLATFORM = BACKGROUND + 1,
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
	TEXTURE_COUNT = TUTORIAL1 + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
	COLOURED = 0,
	SPIKE = COLOURED + 1, // can reuse if we end up having meshes
	TEXTURED = SPIKE + 1,
	WATER = TEXTURED + 1,
	EFFECT_COUNT = WATER + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPIKE = 0,
	SPRITE = SPIKE + 1,
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
	MODE_COUNT = ATTACK + 1
};
const int animation_mode_count = (int)ANIMATION_MODE::MODE_COUNT;

struct SpriteSheet
{
	float timer_ms = 0.f;
	float switchTime_ms;
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

		double maxCount = *std::max_element(spriteCount.begin(), spriteCount.end());
		spriteDim.x = 1.f / maxCount;
		spriteDim.y = 1.f / animation_mode_count;

		updateAnimation(defaultMode);
	}

	void updateAnimation(ANIMATION_MODE newMode) {
		mode = newMode;
		if ((int)mode >= 0) {
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