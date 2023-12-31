// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <tuple>
#include <iostream>
#include <string.h>
#include <fstream>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

#include "physics_system.hpp"

// Game configuration

// Create the fish world
WorldSystem::WorldSystem()
	: points(0)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (player_death_sound != nullptr)
		Mix_FreeChunk(player_death_sound);
	if (student_disappear_sound != nullptr)
		Mix_FreeChunk(student_disappear_sound);
	if (player_jump_sound != nullptr)
		Mix_FreeChunk(player_jump_sound);
	if (player_land_sound != nullptr)
		Mix_FreeChunk(player_land_sound);
	if (collect_book_sound != nullptr)
		Mix_FreeChunk(collect_book_sound);
	if (zombie_kill_sound != nullptr)
		Mix_FreeChunk(zombie_kill_sound);
	if (level_success_sound != nullptr)
		Mix_FreeChunk(level_success_sound);
	if (next_level_sound != nullptr)
		Mix_FreeChunk(next_level_sound);
	if (collected_sound != nullptr)
		Mix_FreeChunk(collected_sound);
	if (boss_summon_sound != nullptr)
		Mix_FreeChunk(boss_summon_sound);
    if (button_hover_sound != nullptr)
		Mix_FreeChunk(button_hover_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace
{
	void glfw_err_cb(int error, const char* desc)
	{
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window()
{
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "UBZ", nullptr, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3)
		{ ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1)
		{ ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2)
		{ ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("tutorial.wav").c_str());
	player_death_sound = Mix_LoadWAV(audio_path("player_death.wav").c_str());
	student_disappear_sound = Mix_LoadWAV(audio_path("student_disappear_quiet.wav").c_str());
	player_jump_sound = Mix_LoadWAV(audio_path("player_jump.wav").c_str());
	player_land_sound = Mix_LoadWAV(audio_path("player_land.wav").c_str());
	collect_book_sound = Mix_LoadWAV(audio_path("Mario-coin-sound.wav").c_str());
	zombie_kill_sound = Mix_LoadWAV(audio_path("splat.wav").c_str());
	level_success_sound = Mix_LoadWAV(audio_path("level-success.wav").c_str());
	next_level_sound = Mix_LoadWAV(audio_path("next-level.wav").c_str());
	collected_sound = Mix_LoadWAV(audio_path("collected.wav").c_str());
	boss_summon_sound = Mix_LoadWAV(audio_path("boss-summon.wav").c_str());
    button_hover_sound = Mix_LoadWAV(audio_path("button-hover.wav").c_str());

	if (background_music == nullptr || player_death_sound == nullptr || student_disappear_sound == nullptr || player_jump_sound == nullptr || player_land_sound == nullptr || collect_book_sound == nullptr || zombie_kill_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("beach.wav").c_str(),
			audio_path("soundtrack.wav").c_str(),
			audio_path("player_death.wav").c_str(),
			audio_path("student_disappear_quiet.wav").c_str(),
			audio_path("player_jump.wav").c_str(),
			audio_path("player_land.wav").c_str(),
			audio_path("Mario-coin-sound.wav").c_str(),
			audio_path("library.wav").c_str(),
			audio_path("forest-level.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg)
{
	this->renderer = renderer_arg;

    // get level left off on
	save_state = readJson(SAVE_STATE_FILE);
	curr_level = save_state["current_level"].asInt();
}

void WorldSystem::initGameState() {
    // Set all states to default for current level
	restart_level();
}

void WorldSystem::loadFromSave() {
    curr_level = save_state["current_level"].asInt();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update)
{

	// Game over
	if (registry.zombies.entities.size() < 1 && (num_collectibles > 0 && collectibles_collected >= num_collectibles) && this->game_over == false) {
		if (curr_level != LAB || (curr_level == LAB && boss_active && registry.bosses.entities.size() == 0)) {
			handleGameOver();
		} else if (curr_level == LAB && !boss_active) { // level == LAB, activate the boss after all zombies and collectibles are cleared
			// remove the blockade, transform professor, play sound effect
			assert(registry.bosses.entities.size() > 0);
			debugging.in_full_view_mode = true;
			boss_active = true;
			removeEntity(boss_blockade);
			Mix_PlayChannel(-1, boss_summon_sound, 0);
		}
	}
	else if (curr_level == MMBOSS && registry.bosses.entities.size() == 0 && this->game_over == false) {
		handleGameOver();
	}

	// Updating window title with points
	updateWindowTitle();

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	auto& motion_container = registry.motions;
	gameTimer += elapsed_ms_since_last_update;
	handleRespawn(elapsed_ms_since_last_update);

	Motion& bozo_motion = registry.motions.get(player_bozo);
	std::vector<std::tuple<Motion*, Motion*>> charactersOnMovingPlat = {};

	// If it is a boss level
	if ((curr_level == MMBOSS || (curr_level == LAB && boss_active)) && registry.bosses.has(boss)) {

		if (!registry.zombieDeathTimers.has(hp) && registry.motions.has(hp)) {
			updateHPBar(bossHealth / registry.bosses.get(boss).health * 100);
		}
		if (!registry.zombieDeathTimers.has(boss)) {
			updateBossMotion(bozo_motion, elapsed_ms_since_last_update);
		}
	}

	for (int i = (int)motion_container.components.size() - 1; i >= 0; --i)
	{
		Motion& motion = motion_container.components[i];
		Entity motionEntity = motion_container.entities[i];
		handleWorldCollisions(motion, motionEntity, bozo_motion, motion_container, elapsed_ms_since_last_update);

		// Add book behaviour
		handleWeaponBehaviour(motion, bozo_motion, motionEntity);

		// If entity if a player effect, for example bozo_pointer, move it along with the player
		if (registry.playerEffects.has(motion_container.entities[i]))
		{
			motion.position.x = bozo_motion.position.x;
			motion.position.y = bozo_motion.position.y;
		}

		if (handleTimers(motion, motionEntity, elapsed_ms_since_last_update)) {
			return true;
		}

		handleFadingEntities();
		//handleKeyframeAnimation(elapsed_ms_since_last_update);		

		// For all objects that are standing on a platform that is moving down, re-update the character position
		// TODO: uncomment if we re-introduce moving platforms
		/*
		for (std::tuple<Motion*, Motion*> tuple : charactersOnMovingPlat)
		{
			Motion& object_motion = *std::get<0>(tuple);
			Motion& plat_motion = *std::get<1>(tuple);

			if (plat_motion.velocity.y > 0)
				object_motion.position.y += plat_motion.velocity.y * (elapsed_ms_since_last_update / 1000.f) + 3.f; // +3 tolerance;
		}
		*/
		// !!! TODO: update timers for dying **zombies** and remove if time drops below zero, similar to the death time
	}
	// outside the loop since the logic inside updateSpriteSheetAnimation is just for bozo and the door
	updateSpriteSheetAnimation(bozo_motion, elapsed_ms_since_last_update);

	if (curr_level == BUSLOOP) {
		if (registry.buses.entities.size() > 0) {
			for (Entity entity : bus_array) {
				Motion& bus_motion = registry.motions.get(entity);
				if (bus_motion.velocity.x > 0) {
					bus_motion.reflect[0] = true;
				} else {
					bus_motion.reflect[0] = false;
				}
			}
		}
	}
	return true;
}

void WorldSystem::handleGameOver() {
	this->game_over = true;
	debugging.in_full_view_mode = true;
	Mix_HaltMusic();
	Mix_PlayChannel(-1, level_success_sound, 0);
	// animate door open
	SpriteSheet& door_sheet = registry.spriteSheets.get(door);
	door_sheet.updateAnimation(ANIMATION_MODE::RUN); // "run" is the door opening animation
	printf("You win!\n");

	// option to retry level? (display current and high scores?)

	// calculate score
	auto level_complete_time = Clock::now();
	float time_to_completion = (float)(std::chrono::duration_cast<std::chrono::microseconds>(level_complete_time - level_start_time)).count() / 1000;

	// save level and high score                                                                                                                                                                             
	save_state["current_level"] = curr_level + 1 > max_level ? 0 : curr_level + 1;
	float prev_high_score = save_state["high_scores"][curr_level].isNull() ? FLT_MAX : save_state["high_scores"][curr_level].asFloat();
	if (time_to_completion < prev_high_score) {
		save_state["high_scores"][curr_level] = time_to_completion;
		printf("new high score: %f", time_to_completion);
	}
	writeJson(save_state, SAVE_STATE_FILE);

	// remove zombies, NPC's, wheels, dangerous, and books on level completion
	while (registry.zombies.entities.size() > 0) {
		removeEntity(registry.zombies.entities.back());
	}
	while (registry.humans.entities.size() > 1) { // for the player
		Entity human = registry.humans.entities.back();
		if (human != player_bozo) {
			removeEntity(human);
		}
	}
	while (registry.books.entities.size() > 0) {
		removeEntity(registry.books.entities.back());
	}
	while (registry.wheels.entities.size() > 0) {
		removeEntity(registry.wheels.entities.back());
	}
	while(registry.dangerous.entities.size() > 0) {
		removeEntity(registry.dangerous.entities.back());
	}
}

void WorldSystem::updateWindowTitle() {
	std::stringstream title_ss;
	title_ss << "Weapons: " << points << " ";
	float curr_high_score = save_state["high_scores"][curr_level].asFloat() / 1000;
	if (curr_high_score > 0) { // will be 0 if null
		title_ss << "High Score: " << curr_high_score << " s";
	}
	glfwSetWindowTitle(window, title_ss.str().c_str());
}

void WorldSystem::handleRespawn(float elapsed_ms_since_last_update) {
	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without interfering with the next object to visit
	// (the containers exchange the last element with the current)
	// generate new zombie every 20s
	enemySpawnTimer += elapsed_ms_since_last_update;
	npcSpawnTimer += elapsed_ms_since_last_update;
	vec4 cameraBounds = renderer->getCameraBounds();

	if (!game_over && zombie_spawn_on && enemySpawnTimer / 1000.f > zombie_spawn_threshold
		&& spawn_on && registry.zombies.entities.size() < num_start_zombies) {
		vec2 enemySpawnPos;
		for (int i = 0; i < zombie_spawn_pos.size(); i++)  // try a few times
		{
			int spawnIndex = rng() % zombie_spawn_pos.size();
			enemySpawnPos = zombie_spawn_pos[spawnIndex];

			// only consider spawning off screen when not in full view mode
			bool spawn = false;
			if (!debugging.in_full_view_mode)
			{
				if (enemySpawnPos.x < cameraBounds[0] || enemySpawnPos.x > cameraBounds[2]
					&& enemySpawnPos.y < cameraBounds[1] || enemySpawnPos.y > cameraBounds[3]) {
					spawn = true;
				}
			}
			else { spawn = true; }

			if (spawn)
			{
				createZombie(renderer, enemySpawnPos, ZOMBIE_ASSET[asset_mapping[curr_level]]);
				enemySpawnTimer = 0.f;
				break;
			}
		}
	}

	if (!game_over && student_spawn_on && npcSpawnTimer / 1000.f > student_spawn_threshold 
		&& spawn_on && registry.humans.entities.size() < num_start_students) {
		vec2 npcSpawnPos;
		for (int i = 0; i < npc_spawn_pos.size(); i++)  // try a few times
		{
			int spawnIndex = rng() % npc_spawn_pos.size();
			npcSpawnPos = npc_spawn_pos[spawnIndex];

			// only consider spawning off screen when not in full view mode
			bool spawn = false;
			if (!debugging.in_full_view_mode)
			{
				if (npcSpawnPos.x < cameraBounds[0] || npcSpawnPos.x > cameraBounds[2]
					&& npcSpawnPos.y < cameraBounds[1] || npcSpawnPos.y > cameraBounds[3]) {
					spawn = true;
				}
			}
			else { spawn = true; }

			if (spawn)
			{
				Entity student = createStudent(renderer, npcSpawnPos, NPC_ASSET[asset_mapping[curr_level]]);
				Motion& student_motion = registry.motions.get(student);
				if (curr_level == BUS) {
					student_motion.velocity = { 0.f,0.f };
				}
				else {
					student_motion.velocity.x = uniform_dist(rng) > 0.5f ? 100.f : -100.f;
				}

				npcSpawnTimer = 0.f;
				break;
			}
		}
	}
}

bool WorldSystem::handleTimers(Motion& motion, Entity motionEntity, float elapsed_ms_since_last_update) {
	// Processing the player state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	float min_timer_ms = 3000.f;
	float infect_timer_ms = 3000.f;
	float min_angle = asin(-1);
	float max_angle = asin(1);

	if (registry.deathTimers.has(motionEntity)) {
		// progress timer, make the rotation happening based on time
		DeathTimer& timer = registry.deathTimers.get(motionEntity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if (timer.timer_ms < min_timer_ms)
		{
			// reduce window brightness if player is dying
			screen.screen_darken_factor = 1 - timer.timer_ms / min_timer_ms;
			if (timer.direction == 0)
			{
				if (motion.angle > min_angle)
				{
					motion.angle += asin(-1) / 50;
				}
			}
			else
			{
				if (motion.angle < max_angle)
				{
					motion.angle += asin(1) / 50;
				}
			}
		}

		// restart the game once the death timer expired
		if (timer.timer_ms < 0)
		{
			registry.deathTimers.remove(motionEntity);
			restart_level();
			return true;
		}
	}
	else if (registry.infectTimers.has(motionEntity))
	{
		// progress timer, make the rotation happening based on time
		InfectTimer& timer = registry.infectTimers.get(motionEntity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if (timer.timer_ms < infect_timer_ms)
		{
			infect_timer_ms = timer.timer_ms;
			if (timer.direction == 0)
			{
				if (motion.angle > min_angle)
				{
					motion.angle += asin(-1) / 50;
				}
			}
			else
			{
				if (motion.angle < max_angle)
				{
					motion.angle += asin(1) / 50;
				}
			}
		}

		// remove the NPC player once the timer expires and create a zombie
		if (timer.timer_ms < 0)
		{
			registry.infectTimers.remove(motionEntity);
			Motion lastStudentLocation = registry.motions.get(motionEntity);
			removeEntity(motionEntity);
			Entity new_zombie = createZombie(renderer, lastStudentLocation.position, ZOMBIE_ASSET[asset_mapping[curr_level]]);
		}
	}
	else if (registry.zombieDeathTimers.has(motionEntity)) {
		ZombieDeathTimer& timer = registry.zombieDeathTimers.get(motionEntity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if (timer.timer_ms < 0) {
			removeEntity(motionEntity); // remove zombie (also removes timer)
		}
	}

	else if (registry.cutSceneTimers.has(motionEntity)) {
		CutsceneTimer& cs_timer = registry.cutSceneTimers.get(motionEntity);
		cs_timer.timer -= elapsed_ms_since_last_update;
		if (cs_timer.timer < 0) {
			removeEntity(motionEntity);
			curr_level = curr_level + 1 > max_level ? 0 : curr_level + 1;
			restart_level();
		}
	}

	return false;
}

void WorldSystem::handleWeaponBehaviour(Motion& motion, Motion& bozo_motion, Entity entity) {
	if (registry.books.has(entity))
	{
		Book& book = registry.books.get(entity);
		// If book is in hand, we consider it as on ground and always go with player
		if (book.offHand == false)
		{
			motion.offGround = false;
			motion.position.x = bozo_motion.position.x + BOZO_BB_WIDTH / 2;
			motion.position.y = bozo_motion.position.y;
		}
		// If book is on ground, it's velocity should always be 0
		if (motion.offGround == false)
		{
			motion.velocity = { 0.f, 0.f };
		}
	}
}

void WorldSystem::handleFadingEntities() {
	for (Entity entity : registry.fading.entities)
	{
		// progress timer, make the rotation happening based on time
		// Set fading factor 
		Fading& label = registry.fading.get(entity);
		auto now = Clock::now();

		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - label.fading_timer)).count() / 1000;

		if (elapsed_ms > 3000.f)
		{
			registry.remove_all_components_of(entity);
			break;
		}
		label.fading_factor = cos(0.0005 * elapsed_ms);
	}
}

void WorldSystem::handleKeyframeAnimation(float elapsed_ms_since_last_update) {
	for (Entity entity : registry.keyframeAnimations.entities)
	{
		bool updateVelocity = false;
		KeyframeAnimation& animation = registry.keyframeAnimations.get(entity);
		animation.timer_ms += elapsed_ms_since_last_update;

		// update frame when time limit is reached
		if (animation.timer_ms >= animation.switch_time)
		{
			animation.timer_ms = 0.f;
			animation.curr_frame++;
			updateVelocity = true; // update velocity only when frame switch has occurred
		}

		// ensure we set next frame to first frame if looping animation
		int next = animation.loop ? (animation.curr_frame + 1) % (animation.num_of_frames) : (animation.curr_frame + 1);
		if (next >= animation.num_of_frames)
		{
			if (!animation.loop)
				continue;
		}

		// restart animation if looping
		if (animation.curr_frame >= animation.num_of_frames)
			animation.curr_frame = 0;

		Motion& curr_frame = animation.motion_frames[animation.curr_frame];
		Motion& next_frame = animation.motion_frames[next];
		Motion& entity_motion = registry.motions.get(entity);

		// set velocity so we can update entity velocities that are on top of animated entity (e.g. a platform)
		if (updateVelocity)
		{
			entity_motion.velocity =
			{
				(next_frame.position.x - curr_frame.position.x) / (animation.switch_time / 1000.f),
				(next_frame.position.y - curr_frame.position.y) / (animation.switch_time / 1000.f),
			};
		}

		// interpolate motion based on timer
		if (curr_frame.position != next_frame.position)
			entity_motion.position = curr_frame.position + (next_frame.position - curr_frame.position) * (animation.timer_ms / animation.switch_time);
		if (curr_frame.angle != next_frame.angle)
			entity_motion.angle = curr_frame.angle + (next_frame.angle - curr_frame.angle) * (animation.timer_ms / animation.switch_time);
		if (curr_frame.scale != next_frame.scale)
			entity_motion.scale = curr_frame.scale + (next_frame.scale - curr_frame.scale) * (animation.timer_ms / animation.switch_time);
		if (curr_frame.velocity != next_frame.velocity)
			entity_motion.velocity = curr_frame.velocity + (next_frame.velocity - curr_frame.velocity) * (animation.timer_ms / animation.switch_time);
	}
}

void WorldSystem::updateSpriteSheetAnimation(Motion& bozo_motion, float elapsed_ms_since_last_update) {
	if (!registry.lostLifeTimer.has(player_bozo)) {
		SpriteSheet& spriteSheet = registry.spriteSheets.get(player_bozo);
		if (bozo_motion.climbing)
		{
			spriteSheet.updateAnimation(ANIMATION_MODE::CLIMB);
			spriteSheet.truncation.y = 0.f;
			bozo_motion.scale.y = BOZO_BB_HEIGHT + 17.f;
		}
		else
		{
			if (bozo_motion.velocity.x != 0.f && !bozo_motion.offGround)
				spriteSheet.updateAnimation(ANIMATION_MODE::RUN);
			else if (bozo_motion.velocity.x == 0 || bozo_motion.offGround)
				spriteSheet.updateAnimation(ANIMATION_MODE::IDLE);

			spriteSheet.truncation.y = 0.065f;
			bozo_motion.scale.y = BOZO_BB_HEIGHT;
		}
	}

	// handle animation of door when game is over
	SpriteSheet& door_sheet = registry.spriteSheets.get(door);
	if (game_over && door_sheet.mode != ANIMATION_MODE::ATTACK) {
		Door& door_component = registry.doors.get(door);
		doorOpenTimer += elapsed_ms_since_last_update;
		if (doorOpenTimer > door_component.door_open_timer) {
			doorOpenTimer = 0.f;
			door_sheet.updateAnimation(ANIMATION_MODE::ATTACK); // "attack" the mode where the door is open
		}
	}
}

void WorldSystem::handleWorldCollisions(Motion& motion, Entity motionEntity, Motion& bozo_motion, ComponentContainer<Motion>& motion_container, float elapsed_ms_since_last_update) {
	Player& player = registry.players.get(player_bozo);

	auto& platforms = registry.platforms;
	auto& walls = registry.walls;

	bool isNPC = registry.humans.has(motionEntity) && !registry.players.has(motionEntity);
	bool isPlayer = registry.players.has(motionEntity);
	bool isHuman = isNPC || isPlayer;
	bool isZombie = registry.zombies.has(motionEntity);
	bool isBook = registry.books.has(motionEntity);
	bool isBoss = registry.bosses.has(motionEntity);
	bool isWheel = registry.wheels.has(motionEntity);
	bool isBus = registry.buses.has(motionEntity);

	updateWheelRotation();

	if (isPlayer && !registry.deathTimers.has(motionEntity))
	{
		motion.velocity[0] = 0;

		// If player just lost a life, make invincible for a bit
		if (registry.lostLifeTimer.has(player_bozo)) {
			LostLife& timer = registry.lostLifeTimer.get(player_bozo);
			timer.timer_ms -= elapsed_ms_since_last_update;

			if (timer.timer_ms <= 0) {
				registry.lostLifeTimer.remove(player_bozo);
				SpriteSheet& bozo_sheet = registry.spriteSheets.get(player_bozo);
				bozo_sheet.updateAnimation(ANIMATION_MODE::IDLE);
				bozo_sheet.switchTime_ms /= 2.f; // reset switch time
			}
		}


		if (player.keyPresses[0])
		{
			motion.velocity[0] -= PLAYER_SPEED;
		}
		if (player.keyPresses[1])
		{
			motion.velocity[0] += PLAYER_SPEED;
		}

	}
	// Bounding entities to blocks
	if (isHuman || isZombie || isBook || isWheel || isBoss || isBus)
	{
		float entityRightSide = motion.position.x + abs(motion.scale[0]) / 2.f;
		float entityLeftSide = motion.position.x - abs(motion.scale[0]) / 2.f;
		float entityBottom = motion.position.y + motion.scale[1] / 2.f;
		float entityTop = motion.position.y - motion.scale[1] / 2.f;

		vec4 entityBB = { entityRightSide, entityLeftSide, entityBottom, entityTop };

		// Bounding entities to window
		boundEntitiesToWindow(motion, isPlayer);
		bool offAll = true;

		std::vector<Entity> blocks;
		// only check if the block is close enough
		for (int i = 0; i < platforms.size(); i++)
		{
			Motion& m = motion_container.get(platforms.entities[i]);
			if (abs(motion.position.x - m.position.x) < 200 || abs(motion.position.y - m.position.y) < 200) {
				blocks.push_back(platforms.entities[i]);
			}
		}
		for (int i = 0; i < walls.size(); i++)
		{
			Motion& m = motion_container.get(walls.entities[i]);
			if (abs(motion.position.x - m.position.x) < 200 || abs(motion.position.y - m.position.y) < 200) {
				blocks.push_back(walls.entities[i]);
			}
		}

		if (isZombie) {
			registry.zombies.get(motionEntity).right_side_collision = false;
			registry.zombies.get(motionEntity).left_side_collision = false;
		}

		// handle platform collisions
		for (int i = 0; i < blocks.size(); i++)
		{
			Motion& blockMotion = motion_container.get(blocks[i]);

			float xBlockLeftBound = blockMotion.position.x - blockMotion.scale[0] / 2.f;
			float xBlockRightBound = blockMotion.position.x + blockMotion.scale[0] / 2.f;
			float yBlockTop = blockMotion.position.y - blockMotion.scale[1] / 2.f;
			float yBlockBottom = blockMotion.position.y + blockMotion.scale[1] / 2.f;

			// Add this check so that the player can pass through platforms when on a ladderd
			if (!motion.climbing)
			{
				// Collision with Top of block
				if (motion.velocity.y >= 0 && entityBottom >= yBlockTop && entityBottom < yBlockTop + 20.f &&
					entityRightSide > xBlockLeftBound && entityLeftSide < xBlockRightBound)
				{
					motion.position.y = yBlockTop - motion.scale[1] / 2.f;
					motion.velocity.y = 0.f;
					motion.offGround = false;
					offAll = offAll && false;
				}

				// Collision with Bottom of block
				if (motion.velocity.y <= 0 && entityTop < yBlockBottom && entityTop > yBlockBottom - 20.f &&
					entityRightSide > xBlockLeftBound && entityLeftSide < xBlockRightBound)
				{
					motion.position.y = yBlockBottom + motion.scale[1] / 2.f;
					motion.velocity.y = 0.f;
				}
			}

			// Collision with Right edge of block
			if (entityLeftSide < xBlockRightBound &&
				entityLeftSide > xBlockRightBound - abs(motion.scale[0]) / 3.f &&
				entityTop < yBlockBottom &&
				entityBottom > yBlockTop && (player.keyPresses[0] || isZombie || isNPC || isWheel || isBook || isBoss))
			{
				if (isNPC || isWheel) {
					if (registry.platforms.has(blocks[i])) {
						motion.offGround = true;
						motion.velocity[1] -= 50;
					}
					else {
						motion.velocity.x = -motion.velocity.x;
					}
				}
				else if (isZombie) {
					registry.zombies.get(motionEntity).right_side_collision = true;

					if (curr_level == NEST && !motion.offGround)
					{
						motion.offGround = true;
						motion.velocity[1] -= 200;
					}
				}
				else if (isBoss) {
					motion.velocity.x = -motion.velocity.x;
				}
				else {
					motion.velocity.x = 0;
				}
			}

			// Collision with Left edge of block
			if (entityRightSide > xBlockLeftBound &&
				entityRightSide < xBlockLeftBound + abs(motion.scale[0]) / 3.f &&
				entityTop < yBlockBottom &&
				entityBottom > yBlockTop && (player.keyPresses[1] || isZombie || isNPC || isWheel || isBook || isBoss))
			{
				if (isNPC || isWheel) {
					if (registry.platforms.has(blocks[i])) {
						motion.offGround = true;
						motion.velocity[1] -= 50;
					}
					else {
						motion.velocity.x = -motion.velocity.x;
					}
				}
				else if (isZombie) {
					registry.zombies.get(motionEntity).left_side_collision = true;

					if (curr_level == NEST && isZombie && !motion.offGround)
					{
						motion.offGround = true;
						motion.velocity[1] -= 200;
					}
				}
				else if (isBoss) {
					motion.velocity.x = -motion.velocity.x;
				}
				else {
					motion.velocity.x = 0;
				}
			}
		}

		if (motion.climbing)
		{
			motion.offGround = false;
		}
		else
		{
			motion.offGround = offAll;
		}

		if ((isWheel || isNPC) && offAll && !registry.infectTimers.has(motionEntity)) {
			if (motion.velocity.x > 0) {
				motion.position.x -= abs(motion.scale[0] / 3.0f);
			}
			else {
				motion.position.x += abs(motion.scale[0] / 3.0f);
			}
			motion.velocity.x = -motion.velocity.x;
		}

		if (isPlayer)
		{
			updateClimbing(motion, entityBB, motion_container);
		}
		// If entity is a zombie, update its direction to always move towards Bozo
		else if (isZombie && !registry.zombieDeathTimers.has(motionEntity))
		{
			updateZombieMovement(motion, bozo_motion, motionEntity, offAll);
		}
		else if (isNPC)
		{
			if (motion.velocity.x < 0)
			{
				motion.reflect[0] = false;
			}
			else
			{
				motion.reflect[0] = true;
			}
		}

		else {
			if (motion.position.x + abs(motion.scale.x) < 0.f)
			{
				if (isNPC || isWheel) // don't remove the player
					registry.remove_all_components_of(motionEntity);
			}
		}
	}
}

void WorldSystem::boundEntitiesToWindow(Motion& motion, bool isPlayer) {
	if (motion.position.x < BOZO_BB_WIDTH / 2.f && motion.velocity.x < 0 || motion.position.x > window_width_px - BOZO_BB_WIDTH / 2.f && motion.velocity.x > 0)
	{
		if (isPlayer) {
			motion.velocity.x = 0;
		}
		else {
			motion.velocity.x = -motion.velocity.x;
		}
		if (motion.position.y < 0.f + (BOZO_BB_HEIGHT) / 2.f)
		{
			motion.position.y = 0.f + BOZO_BB_HEIGHT / 2.f;
		}
		else if (motion.position.y > window_height_px - BOZO_BB_HEIGHT / 2.f)
		{
			motion.position.y = window_height_px - BOZO_BB_HEIGHT / 2.f;
			motion.velocity.y = 0.f;
			motion.offGround = false;
		}
	}
}

void WorldSystem::updateHPBar(float percent_full) {
	Motion& bossMotion = registry.motions.get(boss);
	Motion& hpBarMotion = registry.motions.get(hp_bar);
	Motion& hpMotion = registry.motions.get(hp);

	hpBarMotion.position[0] = bossMotion.position[0];
	hpBarMotion.position[1] = bossMotion.position[1] - 50;
	hpMotion.position = hpBarMotion.position;

	hpMotion.scale[0] = percent_full / 100 * 80;
	hpMotion.position[0] = hpBarMotion.position[0] - (100 - percent_full) / 100 * 80 / 2;
}

void WorldSystem::updateBossMotion(Motion& bozo_motion, float elapsed_ms_since_last_update) {
	Motion& bossMotion = registry.motions.get(boss);

	if (bossHealth <= 0) {
		// Kill the boss if health reaches 0
		SpriteSheet& boss_spritesheet = registry.spriteSheets.get(boss);
		boss_spritesheet.updateAnimation(ANIMATION_MODE::NINTH_INDEX); // climb is actually death for zombies
		ZombieDeathTimer& timer = registry.zombieDeathTimers.emplace(boss);
		registry.zombieDeathTimers.emplace(hp);
		registry.zombieDeathTimers.emplace(hp_bar);
		timer.timer_ms = 2000;
	}
	else {
		// If boss is not damaged, follow the player
		if (!registry.lostLifeTimer.has(boss)) {

			if (curr_level == MMBOSS || curr_level == LAB) {
				handleBossTimer(bozo_motion, bossMotion, elapsed_ms_since_last_update);
			}

			vec3& color = registry.colors.get(boss);
			color = { 1.f, 1.f, 1.f };

		}
		else {

			// Fade the boss red, and adjust knockback
			LostLife& timer = registry.lostLifeTimer.get(boss);
			timer.timer_ms -= elapsed_ms_since_last_update;

			if (bossMotion.velocity.x > 0) {
				bossMotion.velocity.x -= 5;
			}
			else {
				bossMotion.velocity.x += 5;
			}

			// Make a bit red to show damaged
			vec3& color = registry.colors.get(boss);
			color = { 1.0f, 0.5f, 0.5f };

			if (timer.timer_ms <= 0) {
				registry.lostLifeTimer.remove(boss);
			}

		}
	}
}

void WorldSystem::handleBossTimer(Motion& bozo_motion, Motion& boss_motion, float elapsed_ms_since_last_update) {
	Boss& boss_entity = registry.bosses.get(boss);

	SpriteSheet& mmBossSheet = registry.spriteSheets.get(boss);

	if (boss_entity.summon_timer_ms < 0) {
		boss_motion.climbing = false;
		boss_entity.summon_timer_ms = 13000;

		// Play a summon sound
		Mix_PlayChannel(-1, boss_summon_sound, 0);

		// Spawn some zombozos
		float direction = 1;
		if (boss_motion.velocity.x < 0) {
			float direction = -1;
		}
		boss_motion.velocity.x = 0;
		boss_motion.velocity.y = 0;
		mmBossSheet.updateAnimation(ANIMATION_MODE::FIFTH_INDEX);

		// Make it rain
		if (boss_entity.rain_active == false) {
			for (const auto& rain : mm_boss_rain) {
				Motion& rain_motion = registry.motions.get(rain);
				rain_motion.position.y = 0;
			}
			boss_entity.rain_active = true;
		}

		// Summon a zombie if there are less than 2 on the level
		if (registry.zombies.entities.size() < 2) {
			createZombie(renderer, { boss_motion.position.x + direction * 20, boss_motion.position.y }, ZOMBIE_ASSET[asset_mapping[curr_level]]);
		}

	} else if (boss_entity.summon_timer_ms <= 10000) {
		boss_entity.rain_active = false;
		boss_entity.summon_timer_ms -= elapsed_ms_since_last_update;

		// update movement
		if (curr_level == MMBOSS) {
			updateMainMallBossMovement(bozo_motion, boss_motion);
		} else if (curr_level == LAB) {
			updateLabBossMovement(bozo_motion, boss_motion);
		}
	} else {
		boss_entity.summon_timer_ms -= elapsed_ms_since_last_update;
	}
}

void WorldSystem::updateLabBossMovement(Motion& bozo_motion, Motion& boss_motion)
{
	SpriteSheet& mmBossSheet = registry.spriteSheets.get(boss);
	int bozo_level = checkLevel(bozo_motion);
	int boss_level = checkLevel(boss_motion);

	boss_motion.climbing = false;
	bool reflect = boss_motion.velocity.x < 0;
	if (boss_level == bozo_level ) {
		reflect = (bozo_motion.position.x - boss_motion.position.x) < 0;
	}

	boss_motion.reflect[0] = reflect;

	if (reflect) {
		boss_motion.velocity.x = -MMBOSS_SPEED;
	}
	else {
		boss_motion.velocity.x = MMBOSS_SPEED;
	}
	
	handleJumpPoints(boss_motion, boss_level);

	// This is actually run
	// TODO-Will fix the sprite sheet
	mmBossSheet.updateAnimation(ANIMATION_MODE::ATTACK);
}

void WorldSystem::updateMainMallBossMovement(Motion& bozo_motion, Motion& boss_motion)
{
	int bozo_level = checkLevel(bozo_motion);
	int boss_level = checkLevel(boss_motion);

	SpriteSheet& mmBossSheet = registry.spriteSheets.get(boss);

	if (boss_level == bozo_level)
	{
		boss_motion.climbing = false;
		float direction = -1;
		if ((bozo_motion.position.x - boss_motion.position.x) > 0)
		{
			direction = 1;
		}
		boss_motion.velocity.x = direction * MMBOSS_SPEED;
		if (direction == 1) {
			boss_motion.reflect[0] = false;
		}
		else {
			boss_motion.reflect[0] = true;
		}

		handleJumpPoints(boss_motion, boss_level);

		// This is actually run
		// TODO-Will fix the sprite sheet
		mmBossSheet.updateAnimation(ANIMATION_MODE::ATTACK);

	}
	else if (boss_level < bozo_level)
	{
		// Boss floats to bozo's level

		// Fly up to bozo
		boss_motion.velocity.y = -MMBOSS_SPEED;
		boss_motion.climbing = true;

		// Update to flying animation
		mmBossSheet.updateAnimation(ANIMATION_MODE::SEVENTH_INDEX);
	}
	else
	{
		boss_motion.position.y += 20;
		boss_motion.velocity.y = MMBOSS_SPEED;
		boss_motion.climbing = true;

		// Update to flying animation
		mmBossSheet.updateAnimation(ANIMATION_MODE::SEVENTH_INDEX);
	}
}

void WorldSystem::updateZombieMovement(Motion& motion, Motion& bozo_motion, Entity& zombie, bool offAll)
{
	int bozo_level = checkLevel(bozo_motion);
	int zombie_level = checkLevel(motion);

	if (curr_level == BUS) {
		motion.velocity = { 0.f,0.f };
		return;
	}
	else if (curr_level == MMBOSS) {
		motion.climbing = false;
		float direction = -1;
		if ((bozo_motion.position.x - motion.position.x) > 0)
		{
			direction = 1;
		}
		float speed = ZOMBIE_SPEED;
		motion.velocity.x = direction * speed;

		handleJumpPoints(motion, zombie_level);
	}
	else if (curr_level == SEWERS) {
		float dist = distance(motion.position, bozo_motion.position);
		if (dist < 150.0 && bozo_motion.position.y - 15.f <= motion.position.y) {
			if ((motion.position.x - bozo_motion.position.x) < -10) {
				motion.velocity.x = ZOMBIE_SPEED / 1.f;
			}
			else if ((motion.position.x - bozo_motion.position.x) > 10) {
				motion.velocity.x = -ZOMBIE_SPEED / 1.5f;
			}
			else {
				motion.velocity.x = 0.f;
			}
		}
		else {
			motion.velocity.x = 0.f;
		}

		if (offAll) {
			if (motion.velocity.x > 0) {
				motion.position.x -= 15.f;
			}
			else {
				motion.position.x += 15.f;
			}
			motion.velocity.x = -motion.velocity.x;
		}
	}
	else if (curr_level == NEST && (zombie_level == bozo_level || (bozo_level <= 1 && zombie_level <= 1)))
	{
		// Zombie is on the same level as bozo

		if (curr_level == NEST && bozo_level == 0 && zombie_level == 1 && bozo_motion.position.x < 700)
		{
			float target_ladder = getClosestLadder(zombie_level - 1, bozo_motion);

			if ((target_ladder - motion.position.x) > 0)
			{
				motion.velocity.x = ZOMBIE_SPEED;
				motion.reflect[0] = true;
			}
			else
			{
				motion.velocity.x = -ZOMBIE_SPEED;
				motion.reflect[0] = false;
			}

			// When at the ladder, start descending
			if ((target_ladder - 10.f < motion.position.x && motion.position.x < target_ladder + 10.f))
			{
				motion.position.x = target_ladder;
				motion.velocity.x = 0;
				motion.velocity.y = 2 * ZOMBIE_SPEED;
				motion.climbing = true;
			}
			else
			{
				motion.climbing = false;
			}
		}
		else if (curr_level == NEST && bozo_level == 0 && zombie_level == 0 && bozo_motion.position.x > 700 && motion.position.x < 700)
		{
			float target_ladder = getClosestLadder(zombie_level, bozo_motion);

			if ((target_ladder - motion.position.x) > 0)
			{
				motion.velocity.x = ZOMBIE_SPEED;
				motion.reflect[0] = false;
			}
			else
			{
				motion.velocity.x = -ZOMBIE_SPEED;
				motion.reflect[0] = true;
			}

			// When at the ladder, start ascending
			if ((target_ladder - 10.f < motion.position.x && motion.position.x < target_ladder + 10.f))
			{
				motion.position.x = target_ladder;
				motion.velocity.x = 0;
				motion.velocity.y = -2 * ZOMBIE_SPEED;
				motion.climbing = true;
			}
			else
			{
				motion.climbing = false;
			}
		}
		else
		{
			motion.climbing = false;
			float direction = -1;
			if ((bozo_motion.position.x - motion.position.x) > 0)
			{
				direction = 1;
			}
			float speed = ZOMBIE_SPEED;
			motion.velocity.x = direction * speed;

		}
	}
	else if (zombie_level == bozo_level)
	{
		motion.climbing = false;
		float direction = -1;
		if ((bozo_motion.position.x - motion.position.x) > 0)
		{
			direction = 1;
		}
		float speed = ZOMBIE_SPEED;
		motion.velocity.x = direction * speed;

	} else if (curr_level == LAB && zombie_level != bozo_level) {
		motion.velocity.x = 0;
	}
	else if (zombie_level < bozo_level)
	{
		// Zombie is a level below bozo and needs to climb up

	// Hardcoded exception for basement
		if (curr_level == NEST && zombie_level == 0)
		{
			zombie_level++;
		}

		// Move toward the target_ladder
		float target_ladder = getClosestLadder(zombie_level, bozo_motion);

		if ((target_ladder - motion.position.x) > 0)
		{
			motion.velocity.x = ZOMBIE_SPEED;
			motion.reflect[0] = true;
		}
		else
		{
			motion.velocity.x = -ZOMBIE_SPEED;
			motion.reflect[0] = false;
		}

		// When at the ladder, start ascending
		if ((target_ladder - 10.f < motion.position.x && motion.position.x < target_ladder + 10.f))
		{
			motion.position.x = target_ladder;
			motion.velocity.x = 0;
			motion.velocity.y = -1.2 * ZOMBIE_SPEED;
			motion.climbing = true;
		}
		else
		{
			motion.climbing = false;
		}
	}
	else
	{
		// Zombie is a level above bozo and needs to climb down
		// Move toward the target_ladder
		float target_ladder = getClosestLadder(zombie_level - 1, bozo_motion);

		if ((target_ladder - motion.position.x) > 0)
		{
			motion.velocity.x = ZOMBIE_SPEED;
			motion.reflect[0] = true;
		}
		else
		{
			motion.velocity.x = -ZOMBIE_SPEED;
			motion.reflect[0] = false;
		}

		// When at the ladder, start descending
		if ((target_ladder - 15.f < motion.position.x && motion.position.x < target_ladder + 15.f))
		{
			motion.position.x = target_ladder;
			motion.velocity.x = 0;
			motion.position.y += 20;
			motion.velocity.y = 2 * ZOMBIE_SPEED;
			motion.climbing = true;
		}
		else
		{
			motion.climbing = false;
		}

	}

	handleJumpPoints(motion, zombie_level);

	// update zombie direction
	if (motion.velocity.x > 0 && (zombie_level != bozo_level || abs(motion.position.x - bozo_motion.position.x) > 5)) {
		motion.reflect[0] = true;
	}
	else {
		motion.reflect[0] = false;
	}

    // if ((registry.zombies.get(zombie).right_side_collision && motion.velocity.x < 0) || (registry.zombies.get(zombie).left_side_collision && motion.velocity.x > 0)) {
    if ((registry.zombies.get(zombie).right_side_collision) || (registry.zombies.get(zombie).left_side_collision)) {
        motion.velocity.x = 0;
    }

	// update sprite animation depending on distance to player
	SpriteSheet& zombieSheet = registry.spriteSheets.get(zombie);
	float length = sqrt(abs(motion.position.x - bozo_motion.position.x) + abs(motion.position.y - bozo_motion.position.y));
	if (length < 10.f)
		zombieSheet.updateAnimation(ANIMATION_MODE::ATTACK);
	else {
		if (motion.velocity.x == 0.f) {
			zombieSheet.updateAnimation(ANIMATION_MODE::IDLE);
		}
		else {
			zombieSheet.updateAnimation(ANIMATION_MODE::RUN);
		}
	}
}

void WorldSystem::handleJumpPoints(Motion& motion, int level) {
	if (!motion.offGround) {
		for (float pos : jump_positions[level]) {
			if ((pos - 20.f < motion.position.x && motion.position.x < pos + 20.f))
			{
				motion.velocity.x = 1400;
				motion.velocity.y = -575;
				motion.offGround = true;
			}
		}
	}
}

int WorldSystem::checkLevel(Motion& motion)
{
	float entityBottom = motion.position.y + abs(motion.scale[1]) / 2.f;
	for (int i = 0; i < floor_positions.size() - 1; i++)
	{
		if (entityBottom < floor_positions[i] && entityBottom > floor_positions[i + 1])
			return i;
	}

	return floor_positions.size() - 1;
}

float WorldSystem::getClosestLadder(int zombie_level, Motion& motion)
{
	std::vector<float> ladders = ladder_positions[zombie_level];

	// Find the closest ladder to get to position specified by motion
	int closest = 0;
	float min_dist = 10000;

	for (int i = 0; i < ladders.size(); i++)
	{
		float dist = abs(ladders[i] - motion.position.x);
		if (dist < min_dist)
		{
			closest = i;
			min_dist = dist;
		}
	}

	return ladders[closest];
}

void WorldSystem::updateClimbing(Motion& motion, vec4 entityBB, ComponentContainer<Motion>& motion_container)
{
	Player& player = registry.players.get(player_bozo);
	auto& climbables = registry.climbables;

	bool touchingClimbable = false;

	// This is so that the player can descend the ladder if standing on top
	float entityBottom = motion.position.y + motion.scale[1] / 2.f;

	for (int i = 0; i < climbables.size(); i++)
	{
		Motion& blockMotion = motion_container.get(climbables.entities[i]);

		float xLeftBound = blockMotion.position.x - abs(blockMotion.scale[0]) / 2.f;
		float xRightBound = blockMotion.position.x + abs(blockMotion.scale[0]) / 2.f;
		float yTop = blockMotion.position.y - blockMotion.scale[1] / 2.f;
		float yBottom = blockMotion.position.y + blockMotion.scale[1] / 2.f;

		if (motion.position.x < xRightBound && motion.position.x > xLeftBound && entityBottom < yBottom + 10.f && entityBottom > yTop)
		{
			touchingClimbable = touchingClimbable || true;
		}
	}

	if (touchingClimbable)
	{
		if (player.keyPresses[2] || player.keyPresses[3] || motion.climbing)
		{
			motion.velocity.y = 0;
			if (player.keyPresses[2])
			{
				motion.climbing = true;
				motion.velocity.y -= 200;
			}
			if (player.keyPresses[3] && !isBottomOfLadder({ motion.position.x, entityBottom + motion.scale[1] / 10.f }, motion_container))
			{

				motion.climbing = true;
				motion.velocity.y += 200;
			}
		}
	}
	else
	{
		motion.climbing = false;
	}
}

bool WorldSystem::isBottomOfLadder(vec2 nextPos, ComponentContainer<Motion>& motion_container)
{
	auto& climbables = registry.climbables;

	for (int i = 0; i < climbables.size(); i++)
	{
		Motion& blockMotion = motion_container.get(climbables.entities[i]);

		float xLeftBound = blockMotion.position.x - abs(blockMotion.scale[0]) / 2.f;
		float xRightBound = blockMotion.position.x + abs(blockMotion.scale[0]) / 2.f;
		float yTop = blockMotion.position.y - blockMotion.scale[1] / 2.f;
		float yBottom = blockMotion.position.y + blockMotion.scale[1] / 2.f;

		if (nextPos[0] < xRightBound && nextPos[0] > xLeftBound && nextPos[1] < yBottom && nextPos[1] > yTop)
		{
			return false;
		}
	}
	return true;
}

void WorldSystem::updateWheelRotation()
{
	for (Entity wheel : registry.wheels.entities)
	{
		Motion& wheelMotion = registry.motions.get(wheel);
		float circumference = 2 * M_PI * wheelMotion.scale.x;			 // M_PI is a constant for π
		float distanceTraveled = wheelMotion.velocity.x; // If velocity is per second, multiply by deltaTime
		float rotationRadians = distanceTraveled / circumference * 2 * M_PI;

		wheelMotion.angle += rotationRadians;
		if (abs(wheelMotion.velocity.x) < 15.0){
			wheelMotion.velocity = { 200, 0 };
		}
		
		// 	const float rotationSpeed = 0.0001f;
		// 	if (wheelMotion.velocity.x >= 0)
		// 		wheelMotion.angle += rotationSpeed * wheelMotion.velocity.x;
		// 	else if (wheelMotion.velocity.x < 0)
		// 		wheelMotion.angle += rotationSpeed * wheelMotion.velocity.x;
		// }
	}
}


// Reset the world state to its initial state
void WorldSystem::restart_level()
{
	debugging.in_full_view_mode = false;
	this->game_over = false;
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game state variables
	enemySpawnTimer = 0.f;
	npcSpawnTimer = 0.f;
	doorOpenTimer = 0.f;
	collectibles_collected_pos = 50.f;
	player_lives = 4;
	collectibles_collected = 0;
	boss_active = false;

	// reset screen brightness
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];
	screen.screen_darken_factor = 0;

	// set poisoned to be false
	screen.is_poisoned = false;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		removeEntity(registry.motions.entities.back());

	while (registry.lights.entities.size() > 0)
		registry.remove_all_components_of(registry.lights.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// load from json
	std::ifstream file(LEVEL_DESCRIPTORS[curr_level]);
	file >> jsonData;

	// update BGM
	background_music = Mix_LoadMUS(audio_path(jsonData["bgm"].asString()).c_str());
	Mix_PlayMusic(background_music, -1);
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

	// set paltform dimensions
	PLATFORM_WIDTH = jsonData["platform_scale"]["x"].asFloat();
	PLATFORM_HEIGHT = jsonData["platform_scale"]["y"].asFloat();
	WALL_WIDTH = jsonData["wall-width"].asFloat();

	const Json::Value& playerData = jsonData["player"];
	bozo_start_pos = { playerData["position"]["x"].asFloat(), playerData["position"]["y"].asFloat() };

	// reset camera on restart
	renderer->resetCamera(bozo_start_pos);
	renderer->resetSpriteSheetTracker();


	// Create background first (painter's algorithm for rendering)
	for (std::tuple<TEXTURE_ASSET_ID, float> background : BACKGROUND_ASSET[asset_mapping[curr_level]]) {
		createBackground(renderer, std::get<0>(background), std::get<1>(background));
	}

	bus_array.clear();
	if (curr_level == BUSLOOP) {
		bus_array.push_back(createBus(renderer, { 1322, 720 }, { 230.f, 80.f }, { -300, 0}));
	}

	if (curr_level == SEWERS) 
	{
		glm::vec3 lights[8] =
		{
			{ 15, 700, 1.5f },
			{ 1340, 550, 1.5f },
			{ 927, 670, 1.5f },
			{ 850, 450, 1.7f },
			{ 1400, 150, 1.4f },
			{ 500, 790, 1.7f },
			{ 430, 400, 1.6f },
			{ 630, 30, 1.5f },
		};
		for (vec3 light : lights)
		{
			createLight(renderer, { light.x, light.y }, light.z);
		}

	}

	// AnimateBackgrounds for Main Mall Boss level
	else if (curr_level == MMBOSS) {
		addAnimatedMMBossTextures(renderer);
	}

	// Create platforms
	floor_positions.clear();
	for (const auto pos : jsonData["floor_positions"]) {
		floor_positions.push_back(pos.asFloat());
	}

	for (const auto& platform_data : jsonData["platforms"]) {
		createPlatforms(renderer, platform_data["x"].asFloat(), platform_data["y"].asFloat(), platform_data["num_tiles"].asInt(), PLATFORM_ASSET[asset_mapping[curr_level]], platform_data["visible"].asBool(), { PLATFORM_WIDTH, PLATFORM_HEIGHT });
	}

	// Create stairs
	for (const auto& steps_data : jsonData["stairs"]) {
		createSteps(renderer, { steps_data["x"].asFloat(), steps_data["y"].asFloat() }, { steps_data["scale_x"].asFloat(), steps_data["scale_y"].asFloat() }, steps_data["num_steps"].asInt(), steps_data["step_blocks"].asInt(), steps_data["left"].asBool());
	}

	// Create walls
	for (const auto& wall_data : jsonData["walls"]) {
		createWall(renderer, wall_data["x"].asFloat(), wall_data["y"].asFloat(), WALL_WIDTH, wall_data["height"].asFloat(), wall_data["visible"].asBool());
	}

	// create temporary blockade for the boss in lab level
	if (curr_level == LAB) {
		boss_blockade = createWall(renderer, jsonData["boss-blockade"]["position"][0].asFloat(), jsonData["boss-blockade"]["position"][1].asFloat(), jsonData["boss-blockade"]["scale"][0].asFloat(), jsonData["boss-blockade"]["scale"][1].asFloat(), true, TEXTURE_ASSET_ID::LAB_BLOCKADE);
	}

	door = createDoor(renderer, { jsonData["door"]["position"][0].asFloat(), jsonData["door"]["position"][1].asFloat() }, { jsonData["door"]["scale"][0].asFloat(), jsonData["door"]["scale"][1].asFloat() }, DOOR_ASSET[asset_mapping[curr_level]]);

	// Create climbables
	for (const auto& data : jsonData["climbables"]) {
		createClimbable(renderer, data["x"].asFloat(), data["y"].asFloat(), data["sections"].asInt(), CLIMBABLE_ASSET[asset_mapping[curr_level]]);
	}

	total_collectables = jsonData["total_collectables"].asInt();

	ladder_positions.clear();
	for (const auto levelPoints : jsonData["zombie_climb_points"]) {
		std::vector<float> level_climb_points;
		for (const auto point : levelPoints) {
			level_climb_points.push_back(point.asFloat());
		}
		ladder_positions.push_back(level_climb_points);
	}

	jump_positions.clear();
	for (const auto levelPoints : jsonData["zombie_jump_points"]) {
		std::vector<float> level_jump_points;
		for (const auto point : levelPoints) {
			level_jump_points.push_back(point.asFloat());
		}
		jump_positions.push_back(level_jump_points);
	}

	// Create spikes
	for (const auto& spikeData : jsonData["spikes"]) {
		Entity spike = createSpike(renderer, { spikeData["x"].asFloat(), spikeData["y"].asFloat() });
		registry.colors.insert(spike, { spikeData["colour"][0].asFloat(), spikeData["colour"][1].asFloat(), spikeData["colour"][2].asFloat() });
		Motion& spikeMotion = registry.motions.get(spike);
		spikeMotion.angle -= spikeData["angle"].asFloat();
	}

	// Create wheels
	for (const auto& data : jsonData["wheels"]) {
		Entity wheel = createWheel(renderer, { data["position"][0].asFloat(), data["position"][1].asFloat() });
		// registry.colors.insert(wheel, { data["colour"][0].asFloat(), data["colour"][1].asFloat(), data["colour"][2].asFloat() });
		Motion& wheelMotion = registry.motions.get(wheel);
		wheelMotion.velocity = { data["velocity"][0].asFloat(), data["velocity"][1].asFloat() };
		wheelMotion.scale = wheelMotion.scale * data["size"].asFloat();
	}

	// Create boss
	const Json::Value& bossData = jsonData["boss"];
	uint num_starting_bosses = bossData["num_starting"].asInt();
	if (num_starting_bosses > 0) {
		const auto& boss_pos = bossData["position"];
		vec2 boss_start_pos = { boss_pos["x"].asFloat(), boss_pos["y"].asFloat() };
		vec2 boss_scale = { bossData["scale"]["x"].asFloat(), bossData["scale"]["y"].asFloat() };
		vec2 boss_trunc = { bossData["trunc"]["x"].asFloat(), bossData["trunc"]["y"].asFloat() };
		std::vector<int> spriteCounts;
		for (const auto& count : bossData["sprite_counts"]) {
			spriteCounts.push_back(count.asInt());
		}
		boss = createBoss(renderer, boss_start_pos, boss_scale, bossData["health"].asFloat(), bossData["damage"].asFloat(), BOSS_ASSET[asset_mapping[curr_level]], boss_trunc, spriteCounts);
		hp = createHP(renderer, registry.motions.get(boss).position);
		hp_bar = createHPBar(renderer, registry.motions.get(boss).position);
		bossHealth = registry.bosses.get(boss).health;
	}

	// Create a new Bozo player
	player_bozo = createBozo(renderer, bozo_start_pos);
	registry.colors.insert(player_bozo, { 1, 0.8f, 0.8f });

	// Create aiming arrow for player
	player_bozo_pointer = createBozoPointer(renderer, { 200, 500 });

	// Create zombies
	zombie_spawn_pos.clear();
	num_start_zombies = jsonData["zombies"]["num_starting"].asInt(); // so that zombie positions are separate from how many start
	assert(num_start_zombies <= jsonData["zombies"]["positions"].size());
	uint z = 0;
	for (const auto& zombie_pos : jsonData["zombies"]["positions"]) {
		vec2 pos = { zombie_pos["x"].asFloat(), zombie_pos["y"].asFloat() };
		zombie_spawn_pos.push_back(pos);
		if (z < num_start_zombies) {
			createZombie(renderer, pos, ZOMBIE_ASSET[asset_mapping[curr_level]]);
		}
		z++;
	}
	// Set zombie spawn timer if not null
	if (jsonData["zombies"]["spawn_timer"]) {
		zombie_spawn_threshold = jsonData["zombies"]["spawn_timer"].asFloat();
		zombie_spawn_on = true;
	}
	else {
		zombie_spawn_on = false;
	}

	// Create students
	npc_spawn_pos.clear();
	num_start_students = jsonData["students"]["num_starting"].asInt();
	assert(num_start_students <= jsonData["students"]["positions"].size());
	uint s = 0;
	for (const auto& student_pos : jsonData["students"]["positions"]) {
		vec2 pos = { student_pos["x"].asFloat(), student_pos["y"].asFloat() };
		npc_spawn_pos.push_back(pos);
		if (s < num_start_students) {
			Entity student = createStudent(renderer, pos, NPC_ASSET[asset_mapping[curr_level]]);
			// coded back+forth motion
			Motion& student_motion = registry.motions.get(student);
			if (curr_level == BUS) {
				student_motion.velocity = { 0.f,0.f };
			}
			else {
				student_motion.velocity.x = uniform_dist(rng) > 0.5f ? 100.f : -100.f;
			}
		}
		s++;
	}
	// Set student spawn timer if not null
	if (jsonData["students"]["spawn_timer"]) {
		student_spawn_threshold = jsonData["students"]["spawn_timer"].asFloat();
		student_spawn_on = true;
	}
	else {
		student_spawn_on = false;
	}

	// Place collectibles
	const Json::Value& collectiblesPositions = jsonData["collectibles"]["positions"];
	num_collectibles = collectiblesPositions.size(); // set number of collectibles
	vec2 collectible_scale = { jsonData["collectibles"]["scale"]["x"].asFloat(), jsonData["collectibles"]["scale"]["y"].asFloat() };
	std::vector<TEXTURE_ASSET_ID> collectible_assets = COLLECTIBLE_ASSETS[asset_mapping[curr_level]];
	//print asset_mapping[curr_level]
	printf("asset_mapping[curr_level]: %d\n", asset_mapping[curr_level]);
	printf("curr_level: %d\n", curr_level);	
	printf("num_collectibles: %d, collectible_assets.size(): %d\n", num_collectibles, collectible_assets.size());
	assert(num_collectibles <= collectible_assets.size());
	for (uint i = 0; i < num_collectibles; i++) {
		bool isPoisonous = collectible_assets[i] == TEXTURE_ASSET_ID::FOREST_MUSHROOM ? true : false;
		createCollectible(renderer, collectiblesPositions[i]["x"].asFloat(), collectiblesPositions[i]["y"].asFloat(), collectible_assets[i], collectible_scale, false, isPoisonous);
	}

	// This is specific to the beach level
	if (curr_level == BEACH) {
		createDangerous(renderer, { 280, 130 }, { 30, 30 }, TEXTURE_ASSET_ID::SPIKE_BALL, { 280, 130 }, { 500, 10 }, { 650, 250 }, { 0, 0 }, false, true, 6);
		createDangerous(renderer, { 280, 130 }, { 50, 50 }, TEXTURE_ASSET_ID::BEACH_BIRD, { 0, 400 }, { 500, 50 }, { 1000, 750 }, { 1450, 400 }, true, true, 6);
		createBackground(renderer, TEXTURE_ASSET_ID::CANNON, 0.f, { 230, 155 }, false, { 80, 60 });
	}

	mm_boss_rain.clear();
	if (curr_level == MMBOSS) {
		for (float i = 200; i < 1400; i += 200) {
			Entity dng = createDangerous(renderer, { i, 850 }, { 30, 30 }, TEXTURE_ASSET_ID::MM_RAIN, { 200, 0 }, { 200, 200 }, { 200, 400 }, { 0, 0 }, false, false, 1);
			mm_boss_rain.push_back(dng);
		}
	}
	// Lives can probably stay hardcoded?

	if (jsonData["isCutscene"] == true) {
		playCutscene(renderer);
	}
	else {
		float heart_pos_x = 1385;
		float heart_starting_pos_y = 40;

		Entity heart0 = createHeart(renderer, { heart_pos_x, heart_starting_pos_y }, { 60, 60 });
		Entity heart1 = createHeart(renderer, { heart_pos_x, heart_starting_pos_y + 60 }, { 60, 60 });
		Entity heart2 = createHeart(renderer, { heart_pos_x, heart_starting_pos_y + 120 }, { 60, 60 });
		Entity heart3 = createHeart(renderer, { heart_pos_x, heart_starting_pos_y + 180 }, { 60, 60 });
		Entity heart4 = createHeart(renderer, { heart_pos_x, heart_starting_pos_y + 240 }, { 60, 60 });

		player_hearts = { heart0, heart1, heart2, heart3, heart4 };

		// Create label
		Entity label = createOverlay(renderer, { 100, 600 }, { 150 , 75 }, LABEL_ASSETS[asset_mapping[curr_level]], true);

		setup_keyframes(renderer);
		points = 0;
		level_start_time = Clock::now();
	}
}

void WorldSystem::transitionToMenuState() {
    debugging.in_full_view_mode = false;
	this->game_over = false;
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game state variables
	enemySpawnTimer = 0.f;
	npcSpawnTimer = 0.f;
	doorOpenTimer = 0.f;
	collectibles_collected_pos = 50.f;
	player_lives = 4;
	collectibles_collected = 0;

	// reset screen brightness
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];
	screen.screen_darken_factor = 0;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		removeEntity(registry.motions.entities.back());

	while (registry.lights.entities.size() > 0)
		registry.remove_all_components_of(registry.lights.entities.back());

    screen.is_poisoned = false;

    Mix_HaltMusic();

}

void WorldSystem::addAnimatedMMBossTextures(RenderSystem* renderer)
{
	createAnimatedBackgroundObject(renderer, { 728, 720 }, { 130, 130 }, TEXTURE_ASSET_ID::MM_FOUNTAIN, { 4 }, { 0, 0.01 });
}

void WorldSystem::playCutscene(RenderSystem* renderer) {
	int num_of_sprites = jsonData["num"].asInt();
	float time_of_switch = jsonData["time"].asFloat();
	createCutscene(renderer, { 1080, 608 }, { 720, 405 }, CUTSCENE_ASSET[asset_mapping[curr_level]], { num_of_sprites }, time_of_switch, { 0, 0 });
}

// Compute collisions between entities
void WorldSystem::handle_collisions()
{
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++)
	{
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other_entity;

		// For now, we are only interested in collisions that involve the player
		if (registry.players.has(entity))
		{
			// Checking Player - Zombie collisions TODO: can generalize to Human - Zombie, and treat player as special case
			bool isZombie = registry.zombies.has(entity_other);
			bool isSpikes = registry.spikes.has(entity_other);
			bool isDangerous = registry.dangerous.has(entity_other);
			bool isWheel = registry.wheels.has(entity_other);
			bool isBoss = registry.bosses.has(entity_other);
			bool isBus = registry.buses.has(entity_other);
			if (!game_over &&
				((isZombie && !registry.zombieDeathTimers.has(entity_other)) || isSpikes || isDangerous || isWheel || isBoss || isBus) &&
				!registry.deathTimers.has(entity) &&
				!registry.lostLifeTimer.has(player_bozo))
			{
				// Remove a heart
				registry.remove_all_components_of(player_hearts[player_lives]);

				// Play death sound
				if (isZombie) {
					Mix_PlayChannel(-1, player_death_sound, 0);
				}
				else {
					Mix_PlayChannel(-1, zombie_kill_sound, 0);
				}

				// Decrement the player lives
				player_lives--;

				// if player runs out of lives, use death timer
				if (player_lives < 0 && !registry.deathTimers.has(entity)) {
					// Scream, reset timer, and make the player [dying animation]
					Motion& motion_player = registry.motions.get(entity);
					Motion& motion_zombie = registry.motions.get(entity_other);

					// Add a little jump animation
					motion_player.offGround = true;
					motion_player.velocity[0] = 0.f;
					motion_player.velocity[1] = -100.f;

					registry.deathTimers.emplace(entity);

					// Set the direction of the death
					DeathTimer& timer = registry.deathTimers.get(entity);
					if (motion_zombie.velocity.x < 0)
					{
						timer.direction = 0;
					}
					else
					{
						timer.direction = 1;
					}

				}
				else {
					// Move player back to start
					Motion& bozo_motion = registry.motions.get(player_bozo);
					bozo_motion.position = bozo_start_pos;

					// Add to lost life timer, animate hurt
					SpriteSheet& bozo_sheet = registry.spriteSheets.get(player_bozo);
					bozo_sheet.updateAnimation(ANIMATION_MODE::HURT);
					bozo_sheet.switchTime_ms *= 2.f; // make switch time slower
					registry.lostLifeTimer.emplace(player_bozo);
				}
			}
			// Checking Player - Human collisions
			else if (!game_over && registry.humans.has(entity_other))
			{
				if (!registry.deathTimers.has(entity))
				{
					// random chance of spawning book at the same position as the "saved" student, plays different sound if a book is spawned
					//int spawn_book = rng() % 2; // 0 or 1
					int spawn_book = 1;
					if (spawn_book)
					{
						Motion& m = registry.motions.get(entity_other);
						Entity book = createBook(renderer, m.position, WEAPON_ASSETS[asset_mapping[curr_level]]);
						Book& b = registry.books.get(book);
						b.offHand = false;
						++points;
						Mix_PlayChannel(-1, collect_book_sound, 0);
					}
					else
					{
					}
					removeEntity(entity_other);
					Mix_PlayChannel(-1, student_disappear_sound, 0);
				}
			}
			// Check Player - Book collisions
			else if (!game_over && registry.books.has(entity_other))
			{
				bool& offHand = registry.books.get(entity_other).offHand;
				Motion& motion_book = registry.motions.get(entity_other);
				if (motion_book.offGround == false && offHand == true)
				{
					offHand = false;
					++points;
				}
			}
			// Check Player - Door collision for transition to next level
			else if (game_over && registry.doors.has(entity_other))
			{
				Mix_PlayChannel(-1, next_level_sound, 0);
				curr_level++;
				if (curr_level > max_level) {
					curr_level = 0;
				}
				restart_level();
			}
		}
		// Check NPC - Zombie Collision
		else if (!game_over && registry.humans.has(entity) && registry.zombies.has(entity_other))
		{
			// TODO: students don't always turn into zombies
			int turnIntoZombie = rng() % 2; // 0 or 1

			if (turnIntoZombie)
			{
				if (!registry.infectTimers.has(entity))
				{
					// Get the position of the student entity
					Motion& motion_student = registry.motions.get(entity);
					Motion& motion_zombie = registry.motions.get(entity_other);

					// Scream, reset timer, and make the player [dying animation]

					// Add a little jump animation
					motion_student.offGround = true;
					motion_student.velocity[0] = 0.f;
					motion_student.velocity[1] = -200.f;

					// Modify Student's color
					vec3& color = registry.colors.get(entity);
					color = { 1.0f, 0.f, 0.f };

					registry.infectTimers.emplace(entity);

					// Set the direction of the death
					InfectTimer& timer = registry.infectTimers.get(entity);
					if (motion_zombie.velocity.x < 0)
					{
						timer.direction = 0;
					}
					else
					{
						timer.direction = 1;
					}
					Mix_PlayChannel(-1, player_death_sound, 0);
				}
			}
		}
		// Check Book - Zombie collision
		else if (!game_over && registry.books.has(entity) && registry.zombies.has(entity_other) && !registry.zombieDeathTimers.has(entity_other))
		{
			Motion& motion_book = registry.motions.get(entity);
			// Only collide when book is in air
			if (motion_book.offGround == true)
			{
				Mix_PlayChannel(-1, zombie_kill_sound, 0);

				Motion& motion_zombie = registry.motions.get(entity_other);
				motion_zombie.velocity[0] = 0.f;
				motion_zombie.velocity[1] = 0.f;

				// Modify Student's color
				vec3& color = registry.colors.get(entity_other);
				color = { 1.0f, 0.f, 0.f };

				SpriteSheet& zombie_spritesheet = registry.spriteSheets.get(entity_other);
				zombie_spritesheet.switchTime_ms *= 2.0;
				zombie_spritesheet.updateAnimation(ANIMATION_MODE::HURT);
				registry.zombieDeathTimers.emplace(entity_other);
				removeEntity(entity);
			}
		}

		// Book-Boss collision
		else if (!registry.lostLifeTimer.has(boss) && registry.books.has(entity) && registry.bosses.has(entity_other) && !registry.zombieDeathTimers.has(boss))
		{
			Motion& motion_book = registry.motions.get(entity);
			Motion& boss_motion = registry.motions.get(entity_other);
			if (motion_book.offGround == true)
			{
				Mix_PlayChannel(-1, zombie_kill_sound, 0);
				bossHealth -= 20;

				if (bossHealth > 0) {
					// Make boss invincible for a bit between hits
					if (!registry.lostLifeTimer.has(boss)) {
						auto& timer = registry.lostLifeTimer.emplace(boss);
						timer.timer_ms = 500.f;
					}

					// Determine knockback direction
					if (motion_book.velocity.x > 0) {
						boss_motion.velocity.x = BOSS_KNOCKBACK;
					}
					else {
						boss_motion.velocity.x = -BOSS_KNOCKBACK;
					}
				}
				else {
					boss_motion.velocity.x = 0;
				}

				// Remove the book on collision
				removeEntity(entity);
			}
		}

		// Check Spike - Zombie collision
		else if (!game_over && registry.zombies.has(entity) && registry.spikes.has(entity_other)) {
			removeEntity(entity);
		}

		// Player - Collectible collision

		else if (!game_over && registry.collectible.has(entity) && registry.players.has(entity_other)) {
			Mix_PlayChannel(-1, collected_sound, 0);
			TEXTURE_ASSET_ID id = (TEXTURE_ASSET_ID)registry.collectible.get(entity).collectible_id;
			Entity collectible = createOverlay(renderer, {collectibles_collected_pos, 50}, {60,60}, id, false);
			if (registry.poisonous.has(entity)){
				ScreenState& screen = registry.screenStates.components[0];
				screen.is_poisoned = true;
			}
			removeEntity(entity);

			collectibles_collected++;

			collectibles_collected_pos = collectibles_collected_pos + 60;
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (game_state == PLAYING) {
    Motion& motion = registry.motions.get(player_bozo);
    Player& player = registry.players.get(player_bozo);

    if (!pause && action == GLFW_PRESS && !registry.deathTimers.has(player_bozo))
    {
      if (key == GLFW_KEY_A)
      {
        player.keyPresses[0] = true;
      }
      if (key == GLFW_KEY_D)
      {
        player.keyPresses[1] = true;
      }

      if (key == GLFW_KEY_W)
      {
        player.keyPresses[2] = true;
      }

      if (key == GLFW_KEY_S)
      {
        player.keyPresses[3] = true;
      }

      if (key == GLFW_KEY_SPACE && !motion.offGround && !motion.climbing)
      {
        motion.offGround = true;
        motion.velocity[1] = -600;
        Mix_PlayChannel(-1, player_jump_sound, 0);
      }

      if (key == GLFW_KEY_P) {
        debugging.in_full_view_mode = !debugging.in_full_view_mode;
      }

      if (key == GLFW_KEY_L) {
        curr_level++;
        if (curr_level > max_level) {
          curr_level = 0;
        }

        restart_level();
      }
    }

    if (!pause && action == GLFW_RELEASE && (!registry.deathTimers.has(player_bozo)))
    {
      if (key == GLFW_KEY_A)
      {
        player.keyPresses[0] = false;
      }
      if (key == GLFW_KEY_D)
      {
        player.keyPresses[1] = false;
      }
      if (key == GLFW_KEY_W)
      {
        player.keyPresses[2] = false;
      }
      if (key == GLFW_KEY_S)
      {
        player.keyPresses[3] = false;
      }
    }

    // Resetting game (can be done while paused)
    if (action == GLFW_RELEASE && key == GLFW_KEY_R)
    {
      int w, h;
      glfwGetWindowSize(window, &w, &h);

      pause = false;
      restart_level();
    }

    // Debugging (can be done while paused)
    if (key == GLFW_KEY_BACKSLASH)
    {
      if (action == GLFW_RELEASE)
        debugging.in_full_view_mode = false;
      else
        debugging.in_full_view_mode = true;
    }

    // Get location
    if (action == GLFW_RELEASE && key == GLFW_KEY_M) {
      Motion motion = registry.motions.get(player_bozo);
      printf("%f, %f\n", motion.position.x, motion.position.y);
    }

    // Pause
    if (!pause && action == GLFW_PRESS && key == GLFW_KEY_ENTER && curr_level != CUT_1 && curr_level != CUT_2 && curr_level != CUT_3 && curr_level != CUT_4) {
      pause = true;
      if (pause) {
        pause_ui = createOverlay(renderer, { window_width_px / 2, window_height_px / 2 - 100}, { 300.f, 500.f }, TEXTURE_ASSET_ID::PAUSE, false);
        pause_resume = createOverlay(renderer, { window_width_px / 2, 400 - 100}, { 120, 60 }, TEXTURE_ASSET_ID::BACK_BUTTON, false);
        pause_restart_button = createOverlay(renderer, { window_width_px / 2, 490 - 100}, { 120, 60 }, TEXTURE_ASSET_ID::RETRY_BUTTON, false);
        pause_menu_button = createOverlay(renderer, { window_width_px / 2, 310 - 100}, { 120, 60 }, TEXTURE_ASSET_ID::MENU_BUTTON, false);
        pause_start = Clock::now();
      }
    }
  }
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    vec2 pos = relativePos(mouse_position);

    if (game_state == MENU || game_state == PAUSE) {
        menu_pointer = mouse_position;
    }

	if (game_state == PLAYING && !pause && !registry.deathTimers.has(player_bozo))
	{
		Motion& motion = registry.motions.get(player_bozo_pointer);
        float radians = atan2(pos.y - motion.position.y, pos.x - motion.position.x);
		motion.angle = radians;
		// print mouse position
		printf("Mouse position: %f, %f\n", pos.x, pos.y); 
	}
}

vec2 WorldSystem::relativePos(vec2 mouse_position) {
	vec2 relativePos;
	if (debugging.in_full_view_mode == true) {
		relativePos = mouse_position;
	}
	else {
		vec4 cameraBounds = renderer->getCameraBounds();
		relativePos = { cameraBounds[0] + mouse_position.x / 2.f, cameraBounds[1] + mouse_position.y / 2.f };
	}
	return relativePos;
}

void WorldSystem::on_mouse_button(int button, int action, int mod)
{
    if (game_state == MENU || game_state == PAUSE) {
        printf("Position: %f\n", menu_pointer[0]);
        menu_click_pos = menu_pointer;
    }

	if (registry.deathTimers.has(player_bozo))
	{
		return;
	}

	if (!pause && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		auto& booksRegistry = registry.books;
		for (int i = 0; i < booksRegistry.size(); i++)
		{
			Entity entity = booksRegistry.entities[i];
			Book& book = registry.books.get(entity);
			if (book.offHand == false)
			{
				Motion& motion_book = registry.motions.get(entity);
				Motion& motion_pointer = registry.motions.get(player_bozo_pointer);

				motion_book.velocity.x = 500.f * cos(motion_pointer.angle);
				motion_book.velocity.y = 500.f * sin(motion_pointer.angle);

				motion_book.offGround = true;
				book.offHand = true;
				--points;
				break;
			}
		}
	}
}

// defines keyframes for entities that are animated
void WorldSystem::setup_keyframes(RenderSystem* rendered)
{
	// Example use case

	// TODO(vanessa): currently all platforms using same Motion frames are stacked on top of each other, fix to make adjacent
	// 					need to add walls or some other method of preventing characters from going under moving platforms
	//					reconcile behaviour of moving platforms passing through static platforms
	/*std::vector<Entity> moving_plat = createPlatforms(renderer, { 0.f, 0.f }, 7);
	Motion m1 = Motion(vec2(PLATFORM_WIDTH * 9.2, window_height_px * 0.4));
	Motion m2 = Motion(vec2(PLATFORM_WIDTH * 16.2, window_height_px * 0.4));
	std::vector<Motion> frames = { m1, m2 };

	for (uint i = 0; i < moving_plat.size(); i++) {
		Entity currplat = moving_plat[i];
		registry.keyframeAnimations.emplace(currplat, KeyframeAnimation((int)frames.size(), 3000.f, true, frames));
	}*/

	/*std::vector<Entity> moving_plat2 = createPlatforms(renderer, {0.f, 0.f}, 7);
	Motion m3 = Motion(vec2(PLATFORM_WIDTH * 9.2, window_height_px * 0.6));
	Motion m4 = Motion(vec2(PLATFORM_WIDTH * 9.2, window_height_px * 0.4));
	std::vector<Motion> frames2 = { m3, m4 };

	for (uint i = 0; i < moving_plat2.size(); i++) {
		registry.keyframeAnimations.emplace(moving_plat2[i], KeyframeAnimation((int)frames2.size(), 2000.f, true, frames2));
	}*/
}

Json::Value WorldSystem::readJson(std::string file_name) {
	Json::Value json;
	std::ifstream file(file_name);
	file >> json;
	return json;
}

void WorldSystem::writeJson(Json::Value& json, std::string file_name) {
	Json::StreamWriterBuilder writer;
	std::string jsonString = Json::writeString(writer, json);

	std::ofstream outputFile(file_name);
	if (outputFile.is_open()) {
		outputFile << jsonString;
		outputFile.close();
		printf("data written to %s\n", file_name.c_str());
	}
	else {
		printf("ERROR: unable to open %s for writing\n", file_name.c_str());
	}
}

bool WorldSystem::checkPointerInBoundingBox(Motion& motion, vec2 pointer_pos) {
    float left = motion.position[0] - motion.scale[0] / 2;
    float right = motion.position[0] + motion.scale[0] / 2;
    float top = motion.position[1] - motion.scale[1] / 2;
    float bottom = motion.position[1] + motion.scale[1] / 2;

    return pointer_pos[0] > left && pointer_pos[0] < right && pointer_pos[1] > top && pointer_pos[1] < bottom;
}

void WorldSystem::unPause() {
    pause = !pause;
    registry.remove_all_components_of(pause_ui);
    registry.remove_all_components_of(pause_resume);
    registry.remove_all_components_of(pause_menu_button);
    registry.remove_all_components_of(pause_restart_button);
    menu_click_pos = {0, 0};
}

void WorldSystem::playHover() {
    Mix_PlayChannel(-1, button_hover_sound, 0);
}
