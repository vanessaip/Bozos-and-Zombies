
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "world_init.hpp"
#include "components.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world_system;
	RenderSystem render_system;
	PhysicsSystem physics_system;

	// Initializing window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	render_system.init(window);
    world_system.init(&render_system);
	debugging.in_full_view_mode = true;
	Entity loadingScreen = createLoadingScreen(&render_system, { window_width_px / 2, window_height_px / 2 }, { 2 * window_width_px, 3 * window_height_px });

    Entity playButton = createBackground(&render_system, TEXTURE_ASSET_ID::MENU_PLAY, 0.f, { 700, 600 }, { 100, 50 });
    // Entity loadingScreen = createAnimatedBackgroundObject(&render_system, { 728, 720 }, { 130, 130 }, TEXTURE_ASSET_ID::MM_FOUNTAIN, { 4 }, { 0, 0.01 });
	// variable timestep loop

	auto t = Clock::now();
	float total_elapsed = 0.f;
    while (world_system.game_state == MENU) {

        glfwPollEvents();
        auto now = Clock::now();
        float elapsed_ms =
            ((float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000) - world_system.pause_duration;
        total_elapsed += elapsed_ms;
        t = now;

        Motion& play_button_motion = registry.motions.get(playButton);

        if (world_system.checkPointerInBoundingBox(play_button_motion, world_system.menu_pointer)) {
            play_button_motion.scale = { 120, 60 };
        } else {
            play_button_motion.scale = { 100, 50 };
        }
        
        if (world_system.checkPointerInBoundingBox(play_button_motion, world_system.menu_click_pos)) {
            break;
        }

        render_system.step(elapsed_ms);
        render_system.drawMenu(elapsed_ms);
    }

    world_system.initGameState();

    debugging.in_full_view_mode = false;
    registry.remove_all_components_of(loadingScreen);
    registry.remove_all_components_of(playButton);

	while (!world_system.is_over()) {

        world_system.game_state = PLAYING;

		while (world_system.pause) {
			glfwPollEvents();
			printf("PAUSING \n");
		}
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			((float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000) - world_system.pause_duration;
		total_elapsed += elapsed_ms;
		t = now;
		// printf("pause duration: %f ", world_system.pause_duration);
		world_system.pause_duration = 0.f;
		// printf("elapsed time: %f\n", elapsed_ms);

        world_system.step(elapsed_ms);
        physics_system.step(elapsed_ms);
        render_system.step(elapsed_ms);
        world_system.handle_collisions();

		render_system.draw(elapsed_ms);
	}

	return EXIT_SUCCESS;
}
