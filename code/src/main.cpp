
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
	Entity loadingScreen;
    Entity playButton;

	auto t = Clock::now();
	float total_elapsed = 0.f;

    while (!world_system.is_over()) {

        if (world_system.prev_state == PAUSE && world_system.game_state == MENU) {
            loadingScreen = createLoadingScreen(&render_system, { window_width_px / 2, window_height_px / 2 }, { 2 * window_width_px, 3 * window_height_px });
            playButton = createBackground(&render_system, TEXTURE_ASSET_ID::PLAY_BUTTON, 0.f, { window_width_px / 2, 600 }, false, { 120, 60 });
        }

        // ------------------------ GAME STATE MENU ------------------------
        while (world_system.game_state == MENU && !world_system.is_over()) {
            glfwPollEvents();
            auto now = Clock::now();
            total_elapsed = 0;
            t = now;

            Motion& play_button_motion = registry.motions.get(playButton);

            if (world_system.checkPointerInBoundingBox(play_button_motion, world_system.menu_pointer)) {
                play_button_motion.scale = { 150, 75 };
            } else {
                play_button_motion.scale = { 120, 60};
            }
            
            if (world_system.checkPointerInBoundingBox(play_button_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                break;
            }

            render_system.drawMenu(0);
        }

        if (world_system.prev_state == MENU) {

            debugging.in_full_view_mode = false;
            registry.remove_all_components_of(loadingScreen);
            registry.remove_all_components_of(playButton);

            world_system.game_state = PLAYING;

            world_system.initGameState();
        }

        // ------------------------ GAME STATE PLAYING ------------------------
        while (world_system.game_state == PLAYING && !world_system.is_over()) {

            if (world_system.pause) {
                world_system.game_state = PAUSE;
                world_system.prev_state = PLAYING;
                break;
            }
            // Processes system messages, if this wasn't present the window would become unresponsive
            glfwPollEvents();

            // Calculating elapsed times in milliseconds from the previous iteration
            auto now = Clock::now();
            float elapsed_ms =
                ((float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000) - world_system.pause_duration;
            total_elapsed += elapsed_ms;
            t = now;
            world_system.pause_duration = 0.f;
            // printf("pause duration: %f ", world_system.pause_duration);
            // printf("elapsed time: %f\n", elapsed_ms);

            world_system.step(elapsed_ms);
            physics_system.step(elapsed_ms);
            render_system.step(elapsed_ms);
            world_system.handle_collisions();

            render_system.draw(elapsed_ms);
        }

        // ------------------------ GAME STATE PAUSE ------------------------
        while (world_system.game_state == PAUSE && !world_system.is_over()) {
            glfwPollEvents();
            printf("PAUSING \n");

            Motion& resume_motion = registry.motions.get(world_system.pause_resume);
            Motion& menu_motion = registry.motions.get(world_system.pause_menu_button);

            // Button hover logic for "BACK" button
            if (world_system.checkPointerInBoundingBox(resume_motion, world_system.menu_pointer)) {
                resume_motion.scale = { 150, 75 };
            } else {
                resume_motion.scale = { 120, 60 };
            }

            // Button hover logic for "MENU" button
            if (world_system.checkPointerInBoundingBox(menu_motion, world_system.menu_pointer)) {
                menu_motion.scale = { 150, 75 };
            } else {
                menu_motion.scale = { 120, 60 };
            }
            
            // If the "BACK" button is clicked, return to game
            if (world_system.checkPointerInBoundingBox(resume_motion, world_system.menu_click_pos)) {\
                world_system.unPause();
                world_system.game_state = PLAYING;
                world_system.prev_state = PAUSE;
                world_system.menu_click_pos = {0, 0};
                break;
            }

            // If the "MENU" button is clicked, go to the main menu
            if (world_system.checkPointerInBoundingBox(menu_motion, world_system.menu_click_pos)) {\
                world_system.unPause();
                world_system.game_state = MENU;
                world_system.prev_state = PAUSE;
                world_system.menu_click_pos = {0, 0};
                break;
            }

            render_system.drawMenu(0);
        }
    }

	return EXIT_SUCCESS;
}
