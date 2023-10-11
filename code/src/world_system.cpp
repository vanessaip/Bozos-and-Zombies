// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Game configuration

// Create the fish world
WorldSystem::WorldSystem()
	: points(0) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
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
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("salmon_dead.wav").c_str(),
			audio_path("salmon_eat.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motion_container = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)

	Motion& bozo_motion = registry.motions.get(player_bozo);

	for (int i = (int)motion_container.components.size() - 1; i >= 0; --i) {
		Motion& motion = motion_container.components[i];

		auto& platforms = registry.platforms;
		auto& walls = registry.walls;

		// Bounding entities to window
		if (registry.players.has(motion_container.entities[i])) {
			if (motion.position.x < 0.f) {
				motion.position.x = 0.f;
			}
			else if (motion.position.x > window_width_px) {
				motion.position.x = window_width_px;
			}
			if (motion.position.y < 0.f) {
				motion.position.y = 0.f;
			}
			else if (motion.position.y > window_height_px - STUDENT_BB_HEIGHT / 2.f) {
				motion.position.y = window_height_px - STUDENT_BB_HEIGHT / 2.f;
				motion.velocity.y = 0.f;
				motion.offGround = false;
			}

			bool offAll = true;

			for (int i = 0; i < platforms.size(); i++) {
				Entity& platform = platforms.entities[i];
				Motion& platMotion = motion_container.get(platform);
				float xPlatLeftBound = platMotion.position.x - platMotion.scale[0] / 2.f;
				float xPlatRightBound = platMotion.position.x + platMotion.scale[0] / 2.f;
				float yPlatPos = platMotion.position.y - (85.f / 2.f); // Hack: half this quantity so that entities don't fall through moving platforms
				if (motion.velocity.y >= 0 && motion.position.y <= yPlatPos && motion.position.y >= yPlatPos - STUDENT_BB_HEIGHT && 
					motion.position.x > xPlatLeftBound && motion.position.x < xPlatRightBound) {

					// Move character with moving platform
					if (registry.animations.has(platform))
						motion.position += platMotion.velocity * (elapsed_ms_since_last_update / 1000.f);

 					motion.position.y = yPlatPos - (85.f / 2.f);
					motion.velocity.y = 0.f;
					motion.offGround = false;
					offAll = offAll && false;
				}
			}

			for (int i = 0; i < walls.size(); i++) {
				Entity& wall = walls.entities[i];
				Motion& wallMotion = motion_container.get(wall);
				float wallLeftBound = 40.f + STUDENT_BB_WIDTH / 2.f;
				float wallRightBound = 1240.f - STUDENT_BB_WIDTH / 2.f;

				// Moving to the left
				if (motion.position.x < wallLeftBound) {
					// motion.velocity.x = 0.f;
					motion.position.x += 5.f;
				}
				if (motion.position.x > wallRightBound) {
					motion.position.x -= 5.f;
				}
			}

			motion.offGround = offAll;
		}
		else {
			if (motion.position.x < 0.f) {
				motion.position.x = 0.f;
				motion.velocity.x = abs(motion.velocity.x);
			}
			else if (motion.position.x > window_width_px) {
				motion.position.x = window_width_px;
				motion.velocity.x = -abs(motion.velocity.x);
			}
			if (motion.position.y < 0.f) {
				motion.position.y = 0.f;
			}
			else if (motion.position.y > window_height_px - STUDENT_BB_HEIGHT / 2.f) {
				motion.position.y = window_height_px - STUDENT_BB_HEIGHT / 2.f;
				motion.velocity.y = 0.f;
				motion.offGround = false;
			}
		}

		if (registry.humans.has(motion_container.entities[i]) && !registry.players.has(motion_container.entities[i])) {
			if (motion.position.x < 0.f ||
				motion.position.x > window_width_px) {
				motion.velocity.x = -motion.velocity.x;
			}
		}
		else {
			if (motion.position.x + abs(motion.scale.x) < 0.f) {
				if (!registry.players.has(motion_container.entities[i])) // don't remove the player
					registry.remove_all_components_of(motion_container.entities[i]);
			}
		}

		// If entity is a zombie, update its direction to always move towards Bozo
		if (registry.zombies.has(motion_container.entities[i])) {
			vec2 direction = bozo_motion.position - motion.position;
			float length = sqrt(direction.x * direction.x + direction.y * direction.y);
			if (length != 0) {  // prevent division by zero
				direction.x /= length;
				direction.y /= length;
			}
			float speed = 100.f;
			motion.velocity.x = direction.x * speed;
			motion.velocity.y = direction.y * speed;
		}
	}

	// Processing the player state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    float min_timer_ms = 3000.f;
	float min_angle = asin(-1);
	float max_angle = asin(1);

	for (Entity entity : registry.deathTimers.entities) {
		// progress timer, make the rotation happening based on time
		DeathTimer& timer = registry.deathTimers.get(entity);
		Motion& motion = registry.motions.get(entity);
		timer.timer_ms -= elapsed_ms_since_last_update;
		if(timer.timer_ms < min_timer_ms){
			min_timer_ms = timer.timer_ms;
			if (timer.direction == 0) {
				if (motion.angle > min_angle) {
					motion.angle += asin(-1) / 50;
				}	
			}
			else {
				if (motion.angle < max_angle) {
                    motion.angle += asin(1) / 50;
				}			
			}
		}

		// restart the game once the death timer expired
		if (timer.timer_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.screen_darken_factor = 0;
            restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present salmons is dying
	screen.screen_darken_factor = 1 - min_timer_ms / 3000;
	
	// update keyframe animated entity motions
	for (Entity entity : registry.animations.entities)
	{
		bool updateVelocity = false;
		KeyframeAnimation& animation = registry.animations.get(entity);
		animation.timer_ms += elapsed_ms_since_last_update;

		// update frame when time limit is reached
		if (animation.timer_ms >= animation.switch_time) {
			animation.timer_ms = 0.f;
			animation.curr_frame++;
			updateVelocity = true; // update velocity only when frame switch has occurred
		}
		
		// ensure we set next frame to first frame if looping animation
		int next = animation.loop ? (animation.curr_frame + 1) % (animation.num_of_frames) : (animation.curr_frame + 1);
		if (next >= animation.num_of_frames) {
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
		if (updateVelocity) {
			entity_motion.velocity =
			{
				(next_frame.position.x - curr_frame.position.x) / (animation.switch_time/1000.f),
				(next_frame.position.y - curr_frame.position.y) / (animation.switch_time/1000.f),
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
	
	// !!! TODO: update timers for dying **zombies** and remove if time drops below zero, similar to the death timer

	return true;
}	

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create background first (painter's algorithm for rendering)
	Entity background = createBackground(renderer);

	// Create platform(s) at set positions, specify width
	// TODO(vanesssa): define array of platform dimensions for each level
	Entity platform0 = createPlatform(renderer, {window_width_px/2, window_height_px-50.f}, window_width_px-60.f);
	Entity platform1 = createPlatform(renderer, {260,600}, 460.f);
	Entity platform2 = createPlatform(renderer, {window_width_px -460.f,600}, 460.f);
	Entity platform3 = createPlatform(renderer, { window_width_px - 500.f,300 }, 300.f);
	Entity platform4 = createPlatform(renderer, {window_width_px/2, 70.f}, window_width_px-60.f);

	// Create walls
	Entity wall0 = createWall(renderer, {40, 500}, 850);
	Entity wall1 = createWall(renderer, {window_width_px -40, 500}, 850);

	// Create a new Bozo player
	player_bozo = createBozo(renderer, { 200, 500 });
	registry.colors.insert(player_bozo, {1, 0.8f, 0.8f});
	Motion& bozo_motion = registry.motions.get(player_bozo);
	bozo_motion.velocity = { 0.f, 0.f };

	// Create zombie (one starter zombie per level?)
	Entity zombie = createZombie(renderer, {0,0});
	// Setting random initial position and constant velocity (can keep random zombie position?)
	Motion& zombie_motion = registry.motions.get(zombie);
	zombie_motion.position = vec2(window_width_px - 200.f,
			50.f + uniform_dist(rng) * (window_height_px - 100.f));

	// Create student
	Entity student = createStudent(renderer, {0,0});
	// Setting random initial position and constant velocity
	Motion& student_motion = registry.motions.get(student);
	student_motion.position =
		vec2(window_width_px - 200.f,
			50.f + uniform_dist(rng) * (window_height_px - 100.f));
	student_motion.velocity.x = uniform_dist(rng) > 0.5f ? 200.f : -200.f;

	setup_keyframes(renderer);
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other_entity;

		// For now, we are only interested in collisions that involve the player
		if (registry.players.has(entity)) {
			//Player& player = registry.players.get(entity);

			// Checking Player - Zombie collisions TODO: can generalize to Human - Zombie, and treat player as special case
			if (registry.zombies.has(entity_other)) {
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity)) {
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
					if (motion_zombie.velocity.x < 0) {
						timer.direction = 0;
					}
					else { timer.direction = 1; }

					Mix_PlayChannel(-1, salmon_dead_sound, 0);

				}
			}
			// Checking Player - Human collisions
			else if (registry.humans.has(entity_other)) {
				if (!registry.deathTimers.has(entity)) {
					// chew, count points, and set the LightUp timer
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, salmon_eat_sound, 0);
					++points;

					// !!! TODO: just colliding with other students immunizes them or require keyboard input from user?
				}
			}
			// Handle Player - Platform stand on collisions
			else if (registry.platforms.has(entity_other)) {
					Motion& motion_bozo = registry.motions.get(entity);
					Motion& motion_plat = registry.motions.get(entity_other);
							float xPlatLeftBound = motion_plat.position.x - motion_plat.scale[0] / 2.f;
							float xPlatRightBound = motion_plat.position.x + motion_plat.scale[0] / 2.f;
							float yPlatPos = motion_plat.position.y - 85.f;
		         			// Stand on platform
						if (motion_bozo.velocity.y >= 0.f && motion_bozo.position.y <= yPlatPos && motion_bozo.position.y >= yPlatPos - STUDENT_BB_HEIGHT &&
							motion_bozo.position.x > xPlatLeftBound && motion_bozo.position.x < xPlatRightBound) {
							// registry.collisions.emplace_with_duplicates(entity_bozo, entity_plat);
							motion_bozo.position.y = yPlatPos;
									motion_bozo.velocity.y = 0.f;
									motion_bozo.offGround = false;
									// offAll = offAll && false;
						}
						else if (motion_bozo.position.x < xPlatLeftBound || motion_bozo.position.x > xPlatRightBound) {
							motion_bozo.offGround = true;
						}
				}			
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	Motion& motion = registry.motions.get(player_bozo);
	Player& player = registry.players.get(player_bozo);

	if (action == GLFW_PRESS && (!registry.deathTimers.has(player_bozo))) {
		if (key == GLFW_KEY_A) {
			motion.velocity[0] -= 400;
		}
		if (key == GLFW_KEY_D) {
			motion.velocity[0] += 400;
		}

		if (key == GLFW_KEY_SPACE && !motion.offGround) {
			motion.offGround = true;
			motion.velocity[1] -= 700;
		}
	}

	if (action == GLFW_RELEASE && (!registry.deathTimers.has(player_bozo))) {
		if (key == GLFW_KEY_A) {
			motion.velocity[0] += 400;
		}
		if (key == GLFW_KEY_D) {
			motion.velocity[0] -= 400;
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(vec2)mouse_position; // dummy to avoid compiler warning
}

// defines keyframes for entities that are animated
void WorldSystem::setup_keyframes(RenderSystem* renderer)
{
	// Example use case

	Entity moving_plat = createPlatform(renderer, { 0.f, 0.f }, 150.f);
	Motion m1 = Motion(vec2(window_width_px - 150, 300));
	Motion m2 = Motion(vec2(window_width_px - 150, 600));
	std::vector<Motion> frames = { m1, m2 };
	registry.animations.emplace(moving_plat, KeyframeAnimation((int)frames.size(), 3000.f, true, frames));

	Entity moving_plat2 = createPlatform(renderer, { 0.f, 0.f }, 150.f);
	Motion m3 = Motion(vec2(150, 300));
	Motion m4 = Motion(vec2(500, 300));
	std::vector<Motion> frames2 = { m3, m4 };
	registry.animations.emplace(moving_plat2, KeyframeAnimation((int)frames.size(), 2000.f, true, frames2));

}
