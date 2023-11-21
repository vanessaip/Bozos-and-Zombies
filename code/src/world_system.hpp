#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// level loading
#include<json/json.h>

#include "render_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

	void updateZombieMovement(Motion& motion, Motion& bozo_motion, Entity& zombie, bool offAll);

	void updateClimbing(Motion& motion, vec4 entityBB, ComponentContainer<Motion>& motion_container);

	int checkLevel(Motion& motion);

	float getClosestLadder(int zombie_level, Motion& bozo_motion);

	bool isBottomOfLadder(vec2 nextPos, ComponentContainer<Motion>& motion_container);
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mod);

	// Sets up keyframe interpolation for given entity
	void setup_keyframes(RenderSystem* rendered);

	void updateWheelRotation(float elapsed_ms_since_last_update);

	// restart level
	void restart_level();

	// OpenGL window handle
	GLFWwindow* window;

	// TODO: could probably use a similar points system for items gained from saving students
	unsigned int points;

	// Game state
	RenderSystem* renderer;
	Entity player_bozo;
	Entity player_bozo_pointer;
	float enemySpawnTimer = 0.f;
	float npcSpawnTimer = 0.f;
	int max_level = 4;
	float collectibles_collected_pos = 50;
	int collectibles_collected = 0;
	vec2 platformDimensions{ 0.f, 0.f }; // unused

	// This is actually 5 lives but 0 indexed.
	int player_lives = 4;
	std::vector<Entity> player_hearts;
	bool game_over;

	// Level definitions
	int curr_level = 0;
	Json::Value jsonData;
	vec2 bozo_start_pos;
	std::vector<vec2> zombie_spawn_pos;
	std::vector<vec2> npc_spawn_pos;
	std::vector<float> floor_positions;
	std::vector<std::vector<float>> ladder_positions;
	std::vector<std::vector<float>> jump_positions;
	float PLATFORM_WIDTH;
	float PLATFORM_HEIGHT;
  vec2 door_win_pos;
  int total_collectables;
	bool zombie_spawn_on;
	bool student_spawn_on;
	float zombie_spawn_threshold;
	float student_spawn_threshold;
	uint num_collectibles;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* player_death_sound;
	Mix_Chunk* student_disappear_sound;
	Mix_Chunk* player_jump_sound;
	Mix_Chunk* player_land_sound;
	Mix_Chunk* collect_book_sound;
	Mix_Chunk* zombie_kill_sound;
  Mix_Chunk* level_success_sound;
  Mix_Chunk* next_level_sound;
  Mix_Chunk* collected_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// Helpers
	vec2 relativePos(vec2 mouse_position);

	// Debugging
	bool spawn_on = true;

};
