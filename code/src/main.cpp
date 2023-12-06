
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
    bool isHovering = false;
    bool firstLoad = true;

	auto t = Clock::now();
	float total_elapsed = 0.f;

    while (!world_system.is_over()) {

        if (world_system.prev_state == PAUSE && world_system.game_state == MENU) {
            world_system.transitionToMenuState();

            loadingScreen = createOverlay(&render_system, { window_width_px / 2, window_height_px / 2 }, { 1440, 810 }, TEXTURE_ASSET_ID::LOADING_SCREEN, false);
            playButton = createOverlay(&render_system, { window_width_px / 2, 600 }, { 120, 60 }, TEXTURE_ASSET_ID::PLAY_BUTTON, false);
        }

        // ------------------------ GAME STATE MENU ------------------------
        while (world_system.game_state == MENU && !world_system.is_over()) {
            glfwPollEvents();

            Motion& play_button_motion = registry.motions.get(playButton);

            if (world_system.checkPointerInBoundingBox(play_button_motion, world_system.menu_pointer)) {
                if (!isHovering) {
                    world_system.playHover();
                    isHovering = true;
                }
                play_button_motion.scale = { 150, 75 };
            } else {
                isHovering = false;
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
            if (!firstLoad) {
                world_system.pause_end = Clock::now();
                world_system.pause_duration = (float)(std::chrono::duration_cast<std::chrono::microseconds>(world_system.pause_end - world_system.pause_start)).count() / 1000;
            }
            firstLoad = false;
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
            Motion& restart_motion = registry.motions.get(world_system.pause_restart_button);

            bool hoveringResume = world_system.checkPointerInBoundingBox(resume_motion, world_system.menu_pointer);
            bool hoveringMenu = world_system.checkPointerInBoundingBox(menu_motion, world_system.menu_pointer);
            bool hoveringRestart = world_system.checkPointerInBoundingBox(restart_motion, world_system.menu_pointer);

            // Button hover logic for "BACK" button
            if (hoveringResume) {
                if (!isHovering) {
                    world_system.playHover();
                    isHovering = true;
                }
                resume_motion.scale = { 150, 75 };
            } else {
                resume_motion.scale = { 120, 60 };
            }

            // Button hover logic for "MENU" button
            if (hoveringMenu) {
                if (!isHovering) {
                    world_system.playHover();
                    isHovering = true;
                }
                menu_motion.scale = { 150, 75 };
            } else {
                menu_motion.scale = { 120, 60 };
            }

            // Button hover logic for "Restart" button
            if (hoveringRestart) {
                if (!isHovering) {
                    world_system.playHover();
                    isHovering = true;
                }
                restart_motion.scale = { 150, 75 };
            } else {
                restart_motion.scale = { 120, 60 };
            }

            if (!hoveringMenu && !hoveringResume && !hoveringRestart) {
                isHovering = false;
            }
            
            // If the "BACK" button is clicked, return to game
            if (world_system.checkPointerInBoundingBox(resume_motion, world_system.menu_click_pos)) {
                world_system.unPause();
                world_system.game_state = PLAYING;
                world_system.prev_state = PAUSE;
                world_system.menu_click_pos = {0, 0};

                world_system.pause_end = Clock::now();
                world_system.pause_duration = (float)(std::chrono::duration_cast<std::chrono::microseconds>(world_system.pause_end - world_system.pause_start)).count() / 1000;
                break;
            }

            // If the "MENU" button is clicked, go to the main menu
            if (world_system.checkPointerInBoundingBox(menu_motion, world_system.menu_click_pos)) {
                world_system.unPause();
                world_system.game_state = MENU;
                world_system.prev_state = PAUSE;
                world_system.menu_click_pos = {0, 0};
                break;
            }

            // If the "RETRY" button is clicked, restart the level
            if (world_system.checkPointerInBoundingBox(restart_motion, world_system.menu_click_pos)) {
                world_system.unPause();
                world_system.game_state = PLAYING;
                // Trick the game into restarting the level
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                break;
            }

            render_system.drawMenu(0);
        }
    }

	return EXIT_SUCCESS;
}
