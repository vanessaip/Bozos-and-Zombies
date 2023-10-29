// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <tuple>
#include <iostream>

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
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
	if (player_jump_sound != nullptr)
		Mix_FreeChunk(player_jump_sound);
	if (player_land_sound != nullptr)
		Mix_FreeChunk(player_land_sound);
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
	glfwWindowHint(GLFW_RESIZABLE, 0);

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
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2); };
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

	background_music = Mix_LoadMUS(audio_path("soundtrack.wav").c_str());
	player_death_sound = Mix_LoadWAV(audio_path("player_death.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());
	player_jump_sound = Mix_LoadWAV(audio_path("player_jump.wav").c_str());
	player_land_sound = Mix_LoadWAV(audio_path("player_land.wav").c_str());

	if (background_music == nullptr || player_death_sound == nullptr || salmon_eat_sound == nullptr || player_jump_sound == nullptr || player_land_sound == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("soundtrack.wav").c_str(),
			audio_path("player_death.wav").c_str(),
			audio_path("salmon_eat.wav").c_str()),
			audio_path("player_jump.wav").c_str(),
			audio_path("player_land.wav").c_str();
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg)
{
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");
	Mix_VolumeMusic(MIX_MAX_VOLUME / 8);

	// Set all states to default
	restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update)
{
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Books: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motion_container = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)

	Player& player = registry.players.get(player_bozo);

	Motion& bozo_motion = registry.motions.get(player_bozo);
	std::vector<std::tuple<Motion*, Motion*>> charactersOnMovingPlat = {};

	if (bozo_motion.velocity.x > 0)
		bozo_motion.scale.x = BOZO_BB_WIDTH;
	else if (bozo_motion.velocity.x < 0)
		bozo_motion.scale.x = -BOZO_BB_WIDTH;

	for (int i = (int)motion_container.components.size() - 1; i >= 0; --i)
	{
		Motion& motion = motion_container.components[i];

		auto& platforms = registry.platforms;
		auto& walls = registry.walls;
		auto& climables = registry.climbables;

		bool isHuman = registry.humans.has(motion_container.entities[i]);
		bool isZombie = registry.zombies.has(motion_container.entities[i]);
		bool isBook = registry.books.has(motion_container.entities[i]);

		// Bounding entities to window
		if (isHuman || isZombie || isBook)
		{
			float entityRightSide = motion.position.x + abs(motion.scale[0]) / 2.f;
			float entityLeftSide = motion.position.x - abs(motion.scale[0]) / 2.f;
			float entityBottom = motion.position.y + motion.scale[1] / 2.f;
			float entityTop = motion.position.y - motion.scale[1] / 2.f;

			vec4 entityBB = { entityRightSide, entityLeftSide, entityBottom, entityTop };

			if (registry.players.has(motion_container.entities[i]) && !registry.deathTimers.has(motion_container.entities[i]))
			{
				motion.velocity[0] = 0;

				if (player.keyPresses[0])
				{
					motion.velocity[0] -= 200;
				}
				if (player.keyPresses[1])
				{
					motion.velocity[0] += 200;
				}
			}

			if (motion.position.x < 40.f + BOZO_BB_WIDTH / 2.f && motion.velocity.x < 0)
			{
				motion.velocity.x = 0;
			}
			else if (motion.position.x > window_width_px - BOZO_BB_WIDTH / 2.f && motion.velocity.x > 0)
			{
				motion.velocity.x = 0;
			}
			if (motion.position.y < 0.f + BOZO_BB_HEIGHT / 2.f)
			{
				motion.position.y = 0.f + BOZO_BB_HEIGHT / 2.f;
			}
			else if (motion.position.y > window_height_px - BOZO_BB_HEIGHT / 2.f)
			{
				motion.position.y = window_height_px - BOZO_BB_HEIGHT / 2.f;
				motion.velocity.y = 0.f;
				motion.offGround = false;
			}

			bool offAll = true;

			std::vector<Entity> blocks;
			for (int i = 0; i < platforms.size(); i++)
			{
				blocks.push_back(platforms.entities[i]);
			}
			for (int i = 0; i < walls.size(); i++)
			{
				blocks.push_back(walls.entities[i]);
			}

			for (int i = 0; i < blocks.size(); i++)
			{
				Motion& blockMotion = motion_container.get(blocks[i]);

				float xBlockLeftBound = blockMotion.position.x - blockMotion.scale[0] / 2.f;
				float xBlockRightBound = blockMotion.position.x + blockMotion.scale[0] / 2.f;
				float yBlockTop = blockMotion.position.y - blockMotion.scale[1] / 2.f;
				float yBlockBottom = blockMotion.position.y + blockMotion.scale[1] / 2.f;

				// Add this check so that the player can pass through platforms when on a ladder
				if (!motion.climbing) {
					// Collision with Top of block
					if (motion.velocity.y >= 0 && entityBottom >= yBlockTop && entityBottom < yBlockTop + 20.f &&
						entityRightSide > xBlockLeftBound && entityLeftSide < xBlockRightBound)
					{
						// Move character with moving block
						if (registry.keyframeAnimations.has(blocks[i]))
						{
							motion.position.x += blockMotion.velocity.x * (elapsed_ms_since_last_update / 1000.f);
							charactersOnMovingPlat.push_back(std::make_tuple(&motion, &blockMotion)); // track collision if platform is moving down
						}

						if (motion.offGround)
						{
							Mix_PlayChannel(-1, player_land_sound, 0);
						}
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
					entityLeftSide > xBlockRightBound - 10.f &&
					entityTop < yBlockBottom &&
					entityBottom > yBlockTop && (player.keyPresses[0] || isZombie))
				{
					motion.velocity.x = 0;

					if (isZombie && !motion.offGround) {
						motion.offGround = true;
						motion.velocity[1] -= 200;
					}
				}

				// Collision with Left edge of block
				if (entityRightSide > xBlockLeftBound &&
					entityRightSide < xBlockLeftBound + 10.f &&
					entityTop < yBlockBottom &&
					entityBottom > yBlockTop && (player.keyPresses[1] || isZombie))
				{
					motion.velocity.x = 0;

					if (isZombie && !motion.offGround) {
						motion.offGround = true;
						motion.velocity[1] -= 200;
					}
				}
			}

			if (motion.climbing) {
				motion.offGround = false;
			}
			else {
				motion.offGround = offAll;
			}

			if (registry.humans.has(motion_container.entities[i])) {
				updateClimbing(motion, entityBB, motion_container);
			}
		}

		if (registry.humans.has(motion_container.entities[i]) && !registry.players.has(motion_container.entities[i]))
		{
			if (motion.position.x < 40.f + STUDENT_BB_HEIGHT / 2.f ||
				motion.position.x > window_width_px - STUDENT_BB_HEIGHT / 2.f - 40.f)
			{
				motion.velocity.x = -motion.velocity.x;
			}
		}
		else
		{
			if (motion.position.x + abs(motion.scale.x) < 0.f)
			{
				if (!registry.players.has(motion_container.entities[i])) // don't remove the player
					registry.remove_all_components_of(motion_container.entities[i]);
			}
		}

		// Add book behaviour
		if (registry.books.has(motion_container.entities[i])) {
			Book& book = registry.books.get(motion_container.entities[i]);
			Motion motion_player = registry.motions.get(player_bozo);
			// If book is in hand, we consider it as on ground and always go with player
			if (book.offHand == false) {
				motion.offGround = false;
				motion.position.x = motion_player.position.x + BOZO_BB_WIDTH / 2;
				motion.position.y = motion_player.position.y;
			}
			// If book is on ground, it's velocity should always be 0
			if (motion.offGround == false) {
				motion.velocity = { 0.f, 0.f };
			}
		}

		// If entity is a zombie, update its direction to always move towards Bozo
		if (registry.zombies.has(motion_container.entities[i])) {
			updateZombieMovement(motion, bozo_motion, motion_container.entities[i]);
		}
	}

	// Processing the player state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	float min_timer_ms = 3000.f;
	float infect_timer_ms = 3000.f;
	float min_angle = asin(-1);
	float max_angle = asin(1);

	for (Entity entity : registry.deathTimers.entities)
	{
		// progress timer, make the rotation happening based on time
		DeathTimer& timer = registry.deathTimers.get(entity);
		Motion& motion = registry.motions.get(entity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if (timer.timer_ms < min_timer_ms)
		{
			min_timer_ms = timer.timer_ms;
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
			registry.deathTimers.remove(entity);
			screen.screen_darken_factor = 0;
			restart_game();
			return true;
		}
	}

	for (Entity entity : registry.infectTimers.entities)
	{
		// progress timer, make the rotation happening based on time
		InfectTimer& timer = registry.infectTimers.get(entity);
		Motion& motion = registry.motions.get(entity);
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
			registry.infectTimers.remove(entity);
			Motion lastStudentLocation = registry.motions.get(entity);
			registry.remove_all_components_of(entity);
			Entity new_zombie = createZombie(renderer, lastStudentLocation.position);
			return true;
		}
	}

	// reduce window brightness if any of the present salmons is dying
	screen.screen_darken_factor = 1 - min_timer_ms / 3000;

	// update keyframe animated entity motions
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

	// For all objects that are standing on a platform that is moving down, re-update the character position
	for (std::tuple<Motion*, Motion*> tuple : charactersOnMovingPlat)
	{
		Motion& object_motion = *std::get<0>(tuple);
		Motion& plat_motion = *std::get<1>(tuple);

		if (plat_motion.velocity.y > 0)
			object_motion.position.y += plat_motion.velocity.y * (elapsed_ms_since_last_update / 1000.f) + 3.f; // +3 tolerance;
	}
	// !!! TODO: update timers for dying **zombies** and remove if time drops below zero, similar to the death timer

	// update animation mode
	SpriteSheet& spriteSheet = registry.spriteSheets.get(player_bozo);
	if (bozo_motion.velocity.x != 0.f && !bozo_motion.offGround)
		spriteSheet.updateAnimation(ANIMATION_MODE::RUN);
	else if (bozo_motion.velocity.x == 0 || bozo_motion.offGround)
		spriteSheet.updateAnimation(ANIMATION_MODE::IDLE);

	return true;
}

void WorldSystem::updateZombieMovement(Motion& motion, Motion& bozo_motion, Entity& zombie) {

	int bozo_level = checkLevel(bozo_motion);
	int zombie_level = checkLevel(motion);

	if ((zombie_level == bozo_level || (bozo_level <= 1 && zombie_level <= 1))) {
		// Zombie is on the same level as bozo
		motion.climbing = false;
		float direction = -1;
		if ((bozo_motion.position.x - motion.position.x) > 0) {
			direction = 1;
		}
		float speed = 100.f;
		motion.velocity.x = direction * speed;

		// If the zombie hasn't gotten to the player yet, set the appropriate direction the zombie is facing
		if (abs(motion.position.x - bozo_motion.position.x) > 5) {
			if (motion.velocity.x > 0) {
				motion.reflect[0] = true;
			}
			else {
				motion.reflect[0] = false;
			}
		}
	}
	else if (zombie_level < bozo_level) {
		// Zombie is a level below bozo and needs to climb up
		if (zombie_level == 0) {
			zombie_level++;
		}

		// Move toward the target_ladder
		float target_ladder = getClosestLadder(zombie_level, bozo_motion);

		if ((target_ladder - motion.position.x) > 0) {
			motion.velocity.x = 100.f;
		}
		else {
			motion.velocity.x = -100.f;
		}

		// When at the ladder, start ascending
		if ((target_ladder - 10.f < motion.position.x && motion.position.x < target_ladder + 10.f)) {
			motion.position.x = target_ladder;
			motion.velocity.x = 0;
			motion.velocity.y = -100.f;
			motion.climbing = true;
		}
		else {
			motion.climbing = false;
		}

	}
	else {
		// Zombie is a level above bozo and needs to climb down
		// Move toward the target_ladder
		float target_ladder = getClosestLadder(zombie_level - 1, bozo_motion);

		if ((target_ladder - motion.position.x) > 0) {
			motion.velocity.x = 100.f;
		}
		else {
			motion.velocity.x = -100.f;
		}

		// When at the ladder, start descending
		if ((target_ladder - 10.f < motion.position.x && motion.position.x < target_ladder + 10.f)) {
			motion.position.x = target_ladder;
			motion.velocity.x = 0;
			motion.velocity.y = 200.f;
			motion.climbing = true;
		}
		else {
			motion.climbing = false;
		}

	}


	// update sprite animation depending on distance to player
	SpriteSheet& zombieSheet = registry.spriteSheets.get(zombie);
	float length = sqrt(abs(motion.position.x - bozo_motion.position.x) + abs(motion.position.y - bozo_motion.position.y));
	if (length < 75.f)
		zombieSheet.updateAnimation(ANIMATION_MODE::ATTACK);
	else
		zombieSheet.updateAnimation(ANIMATION_MODE::RUN);

}

int WorldSystem::checkLevel(Motion& motion) {
	float entityBottom = motion.position.y + abs(motion.scale[1]) / 2.f;
	if (entityBottom < floor_positions[0] && entityBottom > floor_positions[1]) {
		return 0;
	}
	else if (entityBottom < floor_positions[1] && entityBottom > floor_positions[2]) {
		return 1;
	}
	else if (entityBottom < floor_positions[2] && entityBottom > floor_positions[3]) {
		return 2;
	}
	else if (entityBottom < floor_positions[3] && entityBottom > floor_positions[4]) {
		return 3;
	}
	else {
		return 4;
	}
}

float WorldSystem::getClosestLadder(int zombie_level, Motion& bozo_motion) {
	std::vector<float> ladders = ladder_positions[zombie_level];

	// Find the closest ladder to get to bozo
	int closest = 0;
	float min_dist = 10000;

	for (int i = 0; i < ladders.size(); i++) {
		float dist = abs(ladders[i] - bozo_motion.position.x);
		if (dist < min_dist) {
			closest = i;
			min_dist = dist;
		}
	}

	return ladders[closest];
}

void WorldSystem::updateClimbing(Motion& motion, vec4 entityBB, ComponentContainer<Motion>& motion_container) {
	Player& player = registry.players.get(player_bozo);
	auto& climbables = registry.climbables;

	bool touchingClimbable = false;

	// This is so that the player can descend the ladder if standing on top
	float entityBottom = motion.position.y + motion.scale[1] / 2.f;

	for (int i = 0; i < climbables.size(); i++) {
		Motion& blockMotion = motion_container.get(climbables.entities[i]);

		float xLeftBound = blockMotion.position.x - abs(motion.scale[0]) / 2.f;
		float xRightBound = blockMotion.position.x + abs(motion.scale[0]) / 2.f;
		float yTop = blockMotion.position.y - motion.scale[1] / 2.f;
		float yBottom = blockMotion.position.y + motion.scale[1] / 2.f;

		if (motion.position.x  < xRightBound && motion.position.x > xLeftBound && entityBottom < yBottom && entityBottom > yTop) {
			touchingClimbable = touchingClimbable || true;
		}
	}

	if (touchingClimbable) {
		if (player.keyPresses[2] || player.keyPresses[3] || motion.climbing) {
			motion.velocity.y = 0;
			if (player.keyPresses[2]) {
				motion.climbing = true;
				motion.velocity.y -= 200;
			}
			if (player.keyPresses[3]) {
				motion.climbing = true;
				motion.velocity.y += 200;
			}
		}
	}
	else {
		motion.climbing = false;
	}
}

// Reset the world state to its initial state
void WorldSystem::restart_game()
{
	int curr_level = 0;
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Reset sprite sheet buffer index

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// reset camera on restart
	renderer->resetCamera();
	renderer->resetSpriteSheetTracker();

	// Create background first (painter's algorithm for rendering)
	// base colour
	Entity background = createBackground(renderer);

	// indoor background
	Entity indoor = createBackground(renderer, TEXTURE_ASSET_ID::BACKGROUND_INDOOR);

	// egg
	Entity egg0 = createBackground(renderer, TEXTURE_ASSET_ID::EGG0, { window_width_px / 2 - 80.f, window_height_px * 0.4 }, { 250.f,250.f });

	// Create platform(s) at set positions, specify width
	// TODO(vanesssa): define array of platform dimensions for each level
	uint center_x = window_width_px / 2;
	// floors
	std::vector<Entity> platform0 = createPlatforms(renderer, { center_x - PLATFORM_WIDTH * 7.5, window_height_px - 12.f }, 16);
	std::vector<Entity> platform1 = createPlatforms(renderer, { PLATFORM_WIDTH * 4,window_height_px * 0.8 }, 8);
	std::vector<Entity> platform2 = createPlatforms(renderer, { window_width_px - PLATFORM_WIDTH * 6,window_height_px * 0.8 }, 2);
	std::vector<Entity> platform3 = createPlatforms(renderer, { 110.f,window_height_px * 0.6 }, 7);
	std::vector<Entity> platform4 = createPlatforms(renderer, { window_width_px - PLATFORM_WIDTH * 7 - 80.f, window_height_px * 0.6 }, 7);
	std::vector<Entity> platform5 = createPlatforms(renderer, { 110.f,window_height_px * 0.4 }, 7);
	std::vector<Entity> platform6 = createPlatforms(renderer, { window_width_px - PLATFORM_WIDTH * 10 - 80.f, window_height_px * 0.4 }, 10);
	std::vector<Entity> platform7 = createPlatforms(renderer, { 110.f,window_height_px * 0.2 }, 25);

	floor_positions = {
		window_height_px - 12.f,
		window_height_px * 0.8 ,
		window_height_px * 0.6 ,
		window_height_px * 0.4,
		window_height_px * 0.2 };

	// stairs
	std::vector<Entity> step0 = createSteps(renderer, { PLATFORM_WIDTH * 12 - 20.f,window_height_px * 0.8 }, 5, 3, false);
	std::vector<Entity> step1 = createSteps(renderer, { window_width_px - PLATFORM_WIDTH * 6 - STEP_WIDTH * 6,window_height_px * 0.8 + PLATFORM_HEIGHT * 4 }, 5, 2, true);

	// Create walls
	Entity wall0 = createWall(renderer, { 320.f, window_height_px * 0.9 + 10.f }, window_height_px * 0.2 - 10.f);
	Entity wall1 = createWall(renderer, { window_width_px - 320.f, window_height_px * 0.9 + 10.f }, window_height_px * 0.2 - 10.f);
	Entity wall2 = createWall(renderer, { 180.f, window_height_px * 0.7 + 15.f }, window_height_px * 0.2);
	Entity wall3 = createWall(renderer, { window_width_px - 220.f, window_height_px * 0.7 + 15.f }, window_height_px * 0.2);
	Entity wall4 = createWall(renderer, { 80.f, window_height_px * 0.4 - 20.f }, window_height_px * 0.4 + 70.f);
	Entity wall5 = createWall(renderer, { window_width_px - 100.f, window_height_px * 0.4 - 20 }, window_height_px * 0.4 + 70.f);

	// Create climbables
	std::vector<Entity> ladder0 = createClimbable(renderer, { PLATFORM_WIDTH * 9, window_height_px * 0.8 }, 5);
	std::vector<Entity> ladder1 = createClimbable(renderer, { PLATFORM_WIDTH * 7, window_height_px * 0.6 }, 5);
	std::vector<Entity> ladder2 = createClimbable(renderer, { window_width_px - PLATFORM_WIDTH * 6, window_height_px * 0.6 }, 5);
	std::vector<Entity> ladder3 = createClimbable(renderer, { PLATFORM_WIDTH * 4, window_height_px * 0.2 }, 10);
	std::vector<Entity> ladder4 = createClimbable(renderer, { window_width_px - PLATFORM_WIDTH * 4, window_height_px * 0.4 }, 5);
	std::vector<Entity> ladder5 = createClimbable(renderer, { window_width_px - PLATFORM_WIDTH * 9, window_height_px * 0.2 }, 5);

	ladder_positions = {
		{PLATFORM_WIDTH * 9},
		{PLATFORM_WIDTH * 7, window_width_px - PLATFORM_WIDTH * 6},
		{PLATFORM_WIDTH * 4, window_width_px - PLATFORM_WIDTH * 4},
		{PLATFORM_WIDTH * 4, window_width_px - PLATFORM_WIDTH * 9 } };

	// Create spikes
	Entity spike1 = createSpike(renderer, { 260, 625 });
	registry.colors.insert(spike1, { 0.5f, 0.5f, 0.5f });

	// Create a new Bozo player
	player_bozo = createBozo(renderer, { 500, window_height_px * 0.8 - 50.f });
	registry.colors.insert(player_bozo, { 1, 0.8f, 0.8f });
	Motion& bozo_motion = registry.motions.get(player_bozo);
	bozo_motion.velocity = { 0.f, 0.f };

	// Create zombie (one starter zombie per level?)
	Entity zombie = createZombie(renderer, ZOMBIE_START_POS[curr_level]);

	// Create student
	Entity student = createStudent(renderer, { 1000, 440 });
	registry.colors.insert(student, { 1, 0.8f, 0.8f });

	// Setting random initial position and constant velocity
	Motion& student_motion = registry.motions.get(student);
	/*student_motion.position =
		vec2(window_width_px / 2.f,
			50.f + uniform_dist(rng) * (window_height_px - 100.f));
	student_motion.velocity.x = uniform_dist(rng) > 0.5f ? 200.f : -200.f;*/

	// Create book
	Entity book = createBook(renderer, { 500, 500 });
	Motion& book_motion = registry.motions.get(book);
	book_motion.velocity = { 0.f, 0.f };
	book_motion.offGround = false;

	setup_keyframes(renderer);

	points = 0;
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
			// Player& player = registry.players.get(entity);

			// Checking Player - Zombie collisions TODO: can generalize to Human - Zombie, and treat player as special case
			if (registry.zombies.has(entity_other) || (registry.spikes.has(entity_other))) {
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity))
				{
					// Scream, reset timer, and make the player [dying animation]
					Motion& motion_player = registry.motions.get(entity);
					Motion& motion_zombie = registry.motions.get(entity_other);

					// Add a little jump animation
					motion_player.offGround = true;
					motion_player.velocity[0] = 0.f;
					motion_player.velocity[1] = -200.f;

					// Modify Bozo's color
					vec3& color = registry.colors.get(entity);
					color = { 1.0f, 0.f, 0.f };

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

					Mix_PlayChannel(-1, player_death_sound, 0);
				}
			}
			// Checking Player - Human collisions
			else if (registry.humans.has(entity_other))
			{
				if (!registry.deathTimers.has(entity))
				{
					// chew, count points, and set the LightUp timer
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, salmon_eat_sound, 0);
					// !!! TODO: just colliding with other students immunizes them or require keyboard input from user?
				}
			}
			else if (registry.books.has(entity_other)) {
				bool& offHand = registry.books.get(entity_other).offHand;
				Motion& motion_book = registry.motions.get(entity_other);
				if (motion_book.offGround == false && offHand == true) {
					offHand = false;
					++points;
				}
			}
		}
		else if (registry.humans.has(entity) && registry.zombies.has(entity_other))
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

	Motion& motion = registry.motions.get(player_bozo);
	Player& player = registry.players.get(player_bozo);

	if (action == GLFW_PRESS && (!registry.deathTimers.has(player_bozo)))
	{
		if (key == GLFW_KEY_A)
		{
			player.keyPresses[0] = true;
		}
		if (key == GLFW_KEY_D)
		{
			player.keyPresses[1] = true;
		}

		if (key == GLFW_KEY_W) {
			player.keyPresses[2] = true;
		}

		if (key == GLFW_KEY_S) {
			player.keyPresses[3] = true;
		}

		if (key == GLFW_KEY_SPACE && !motion.offGround)
		{
			motion.offGround = true;
			motion.velocity[1] -= 200;
			Mix_PlayChannel(-1, player_jump_sound, 0);
		}
	}

	if (action == GLFW_RELEASE && (!registry.deathTimers.has(player_bozo)))
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

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_BACKSLASH)
	{
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	{
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	{
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::on_mouse_button(int button, int action, int mod) {
	if (registry.deathTimers.has(player_bozo)) {
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		auto& booksRegistry = registry.books;
		for (int i = 0; i < booksRegistry.size(); i++) {
			Entity entity = booksRegistry.entities[i];
			Book& book = registry.books.get(entity);
			if (book.offHand == false) {
				Motion& motion_book = registry.motions.get(entity);
				Motion& motion_bozo = registry.motions.get(player_bozo);

				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				vec2& position = motion_bozo.position;
				double direction = atan2(ypos - position[1], xpos - position[0]);

				motion_book.velocity.x = 500.f * cos(direction);
				motion_book.velocity.y = 500.f * sin(direction);

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
	Motion m1 = Motion(vec2(window_width_px - PLATFORM_WIDTH*5, window_height_px*0.8));
	Motion m2 = Motion(vec2(window_width_px - PLATFORM_WIDTH*5, window_height_px*0.2));
	std::vector<Motion> frames = { m1, m2 };

	for (uint i = 0; i < moving_plat.size(); i++) {
		Entity currplat = moving_plat[i];
		registry.keyframeAnimations.emplace(currplat, KeyframeAnimation((int)frames.size(), 3000.f, true, frames));
	}

	std::vector<Entity> moving_plat2 = createPlatforms(renderer, { 0.f, 0.f }, 7);
	Motion m3 = Motion(vec2(PLATFORM_WIDTH * 6, window_height_px * 0.8));
	Motion m4 = Motion(vec2(PLATFORM_WIDTH * 6, window_height_px * 0.2));
	std::vector<Motion> frames2 = { m3, m4 };

	for (uint i = 0; i < moving_plat2.size(); i++) {
		registry.keyframeAnimations.emplace(moving_plat2[i], KeyframeAnimation((int)frames.size(), 2000.f, true, frames2));
	}*/
}
