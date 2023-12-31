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

enum game_state {
	MENU = 0,
    PLAYING = 1,
    PAUSE = 2,
};

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	bool pause = false; // TODO(vanessa): enum for playing, pause, game over?
	std::chrono::time_point<std::chrono::steady_clock> pause_start;
	std::chrono::time_point<std::chrono::steady_clock> pause_end;
	float pause_duration = 0.f;
    int game_state = MENU;
    int prev_state = PAUSE;

    // Menu ui 
    vec2 menu_pointer;
    vec2 menu_click_pos;

    // Pause ui
    Entity pause_ui;
    Entity pause_resume;
    Entity pause_menu_button;
    Entity pause_restart_button;

    int curr_level = 0;

	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

  void initGameState();

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

	void updateHPBar(float percent_full);

	void updateBossMotion(Motion& bozo_motion, float elapsed_ms_since_last_update);

	void updateZombieMovement(Motion& motion, Motion& bozo_motion, Entity& zombie, bool offAll);

	void updateClimbing(Motion& motion, vec4 entityBB, ComponentContainer<Motion>& motion_container);

	int checkLevel(Motion& motion);

	float getClosestLadder(int zombie_level, Motion& motion);

	bool isBottomOfLadder(vec2 nextPos, ComponentContainer<Motion>& motion_container);

    bool checkPointerInBoundingBox(Motion& motion, vec2 pointer_pos);

    void unPause();

    void transitionToMenuState();

    void playHover();

    void loadFromSave();
private:
	void handleGameOver();
	void updateWindowTitle();
	void handleRespawn(float elapsed_ms_since_last_update);
	bool WorldSystem::handleTimers(Motion& motion, Entity motionEntity, float elapsed_ms_since_last_update);
	void handleWeaponBehaviour(Motion& motion, Motion& bozo_motion, Entity entity);
	void handleFadingEntities();
	void handleKeyframeAnimation(float elapsed_ms_since_last_update);
	void updateSpriteSheetAnimation(Motion& bozo_motion, float elapsed_ms_since_last_update);
	void handleWorldCollisions(Motion& motion, Entity motionEntity, Motion& bozo_motion, ComponentContainer<Motion>& motion_container, float elapsed_ms_since_last_update);
	void boundEntitiesToWindow(Motion& motion, bool isPlayer);
	void handlePlatformCollision(Motion& blockMotion, vec4 entityBB);
	void playCutscene(RenderSystem* renderer);
	void addAnimatedMMBossTextures(RenderSystem* renderer);
	void handleBossTimer(Motion& bozo_motion, Motion& boss_motion, float elapsed_ms_since_last_update);
	void updateMainMallBossMovement(Motion& bozo_motion, Motion& boss_motion);
	void updateLabBossMovement(Motion& bozo_motion, Motion& boss_motion);
	void handleJumpPoints(Motion& motion, int level);

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mod);

	// Sets up keyframe interpolation for given entity
	void setup_keyframes(RenderSystem* rendered);

	void updateWheelRotation();

	Json::Value WorldSystem::readJson(std::string file_name);
	void WorldSystem::writeJson(Json::Value& json, std::string file_name);

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
	Entity door;
	Entity boss_blockade;
	float enemySpawnTimer = 0.f;
	float npcSpawnTimer = 0.f;
	float doorOpenTimer = 0.f;
	int max_level = 13;
	float collectibles_collected_pos = 50;
	int collectibles_collected = 0;
	Entity loadingScreen;
	bool loading = false;
	float gameTimer = 0.f;
	vec2 platformDimensions{ 0.f, 0.f }; // unused
	std::chrono::time_point<std::chrono::steady_clock> level_start_time;
	Json::Value save_state;
	std::vector<Entity> bus_array;
	Entity boss;
	Entity hp_bar;
	Entity hp;
	float bossHealth;

	// This is actually 5 lives but 0 indexed.
	int player_lives = 4;
	std::vector<Entity> player_hearts;
	bool game_over;
	bool boss_active;

	// Level definitions
	Json::Value jsonData;
	vec2 bozo_start_pos;
	std::vector<vec2> zombie_spawn_pos;
	std::vector<vec2> npc_spawn_pos;
	std::vector<float> floor_positions;
	std::vector<std::vector<float>> ladder_positions;
	std::vector<std::vector<float>> jump_positions;
	float PLATFORM_WIDTH;
	float PLATFORM_HEIGHT;
	float WALL_WIDTH;
	vec2 door_win_pos;
	int total_collectables;
	bool zombie_spawn_on;
	bool student_spawn_on;
	float zombie_spawn_threshold;
	float student_spawn_threshold;
	uint num_start_zombies;
	uint num_start_students;
	uint num_collectibles;
	std::vector<Entity> mm_boss_rain;


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
	Mix_Chunk* boss_summon_sound;
    Mix_Chunk* button_hover_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// Helpers
	vec2 relativePos(vec2 mouse_position);

	// Debugging
	bool spawn_on = true;

};
