#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

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
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mod);

	// Sets up keyframe interpolation for given entity
	void setup_keyframes(RenderSystem* rendered);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	// TODO: could probably use a similar points system for items gained from saving students
	unsigned int points;

	// Game state
	RenderSystem* renderer;
	float current_speed;
	Entity player_bozo;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* player_death_sound;
	Mix_Chunk* salmon_eat_sound;
	Mix_Chunk* player_jump_sound;
	Mix_Chunk* player_land_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
