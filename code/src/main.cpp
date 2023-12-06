
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
    world_system.loadFromSave();
	debugging.in_full_view_mode = true;
	Entity loadingScreen;
    Entity ubzTitle;
    Entity playButton;
    Entity lab_level;
    Entity busloop_level;
    Entity sewer_level;
    Entity wreck_level;
    Entity nest_level;
    Entity mainmall_level;
    Entity ikb_level;
    Entity street_level;
    Entity bus_level;
    Entity forest_level;
    std::vector<Entity> menu_entities;
    bool isHovering = false;
    bool firstLoad = true;
    int levelSelected = 0;
    bool isLevelSelected = true;

	auto t = Clock::now();
	float total_elapsed = 0.f;

    while (!world_system.is_over()) {

        if (world_system.prev_state == PAUSE && world_system.game_state == MENU) {
            world_system.transitionToMenuState();
            world_system.menu_click_pos = {0, 0};

            loadingScreen = createOverlay(&render_system, { window_width_px / 2, window_height_px / 2 }, { 1440, 810 }, TEXTURE_ASSET_ID::LOADING_SCREEN, false);
            ubzTitle = createOverlay(&render_system, { window_width_px / 2, window_height_px / 2  - 200}, { 600, 600 }, TEXTURE_ASSET_ID::UBZ_TITLE, false);
            playButton = createOverlay(&render_system, { window_width_px / 2, 350 }, { 160, 80 }, TEXTURE_ASSET_ID::PLAY_BUTTON, false);

            bus_level = createOverlay(&render_system, { window_width_px / 2 - 400, 500 }, { 160, 160 }, TEXTURE_ASSET_ID::BUS_LVL, false);
            street_level = createOverlay(&render_system, { window_width_px / 2 - 200, 500 }, { 160, 160 }, TEXTURE_ASSET_ID::STREET_LVL, false);
            ikb_level = createOverlay(&render_system, { window_width_px / 2, 500 }, { 160, 160 }, TEXTURE_ASSET_ID::IKB_LVL, false);
            mainmall_level = createOverlay(&render_system, { window_width_px / 2 + 200, 500 }, { 160, 160 }, TEXTURE_ASSET_ID::MAINMALL_LVL, false);
            nest_level = createOverlay(&render_system, { window_width_px / 2 + 400, 500 }, { 160, 160 }, TEXTURE_ASSET_ID::NEST_LVL, false);
            wreck_level = createOverlay(&render_system, { window_width_px / 2 - 400, 700 }, { 160, 160 }, TEXTURE_ASSET_ID::WRECK_LVL, false);
            forest_level = createOverlay(&render_system, { window_width_px / 2 - 200, 700 }, { 160, 160 }, TEXTURE_ASSET_ID::FOREST_LVL, false);
            sewer_level = createOverlay(&render_system, { window_width_px / 2, 700 }, { 160, 160 }, TEXTURE_ASSET_ID::SEWER_LVL, false);
            lab_level = createOverlay(&render_system, { window_width_px / 2 + 200, 700 }, { 160, 160 }, TEXTURE_ASSET_ID::LAB_LVL, false);
            busloop_level = createOverlay(&render_system, {  window_width_px / 2 + 400, 700 }, { 160, 160 }, TEXTURE_ASSET_ID::BUSLOOP_LVL, false);
            menu_entities.push_back(playButton);
            menu_entities.push_back(bus_level);
            menu_entities.push_back(street_level);
            menu_entities.push_back(ikb_level);
            menu_entities.push_back(mainmall_level);
            menu_entities.push_back(nest_level);
            menu_entities.push_back(wreck_level);
            menu_entities.push_back(forest_level);
            menu_entities.push_back(sewer_level);
            menu_entities.push_back(lab_level);
            menu_entities.push_back(busloop_level);
        }

        // ------------------------ GAME STATE MENU ------------------------
        while (world_system.game_state == MENU && !world_system.is_over()) {

            Motion& play_button_motion = registry.motions.get(playButton);
            Motion& bus_motion = registry.motions.get(bus_level);
            Motion& busloop_motion = registry.motions.get(busloop_level);
            Motion& street_motion = registry.motions.get(street_level);
            Motion& ikb_motion = registry.motions.get(ikb_level);
            Motion& mainmall_motion = registry.motions.get(mainmall_level);
            Motion& nest_motion = registry.motions.get(nest_level);
            Motion& wreck_motion = registry.motions.get(wreck_level);
            Motion& forest_motion = registry.motions.get(forest_level);
            Motion& sewer_motion = registry.motions.get(sewer_level);
            Motion& lab_motion = registry.motions.get(lab_level);

            bool hoveringSomething = false;
            for (int i = 0; i < menu_entities.size(); i++) {
                Motion& button_motion = registry.motions.get(menu_entities[i]);
                bool hover = world_system.checkPointerInBoundingBox(button_motion, world_system.menu_pointer);
                if (hover) {
                    if (!isHovering) {
                        world_system.playHover();
                    }
                    hoveringSomething = true;
                    isHovering = true;
                    if (i == 0) {
                        button_motion.scale = { 200, 100 };
                    } else {
                        button_motion.scale = { 200, 200 };
                    }
                } else {
                    if (i == 0) {
                        button_motion.scale = { 160, 80 };
                    } else {
                        button_motion.scale = { 160, 160 };
                    }
                }
            }

            if (!hoveringSomething) {
                isHovering = false;
            }

            isLevelSelected = true;
            
            // TODO-Will refactor this if time allows
            if (world_system.checkPointerInBoundingBox(play_button_motion, world_system.menu_click_pos)) {
                isLevelSelected = false;
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                break;
            }

            if (world_system.checkPointerInBoundingBox(bus_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 0;
                break;
            }

            if (world_system.checkPointerInBoundingBox(street_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 2;
                break;
            }

            if (world_system.checkPointerInBoundingBox(ikb_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 4;
                break;
            }

            if (world_system.checkPointerInBoundingBox(mainmall_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 6;
                break;
            }

            if (world_system.checkPointerInBoundingBox(nest_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 7;
                break;
            }

            if (world_system.checkPointerInBoundingBox(wreck_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 8;
                break;
            }

            if (world_system.checkPointerInBoundingBox(forest_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 10;
                break;
            }

            if (world_system.checkPointerInBoundingBox(sewer_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 9;
                break;
            }

            if (world_system.checkPointerInBoundingBox(lab_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 11;
                break;
            }

            if (world_system.checkPointerInBoundingBox(busloop_motion, world_system.menu_click_pos)) {
                world_system.prev_state = MENU;
                world_system.menu_click_pos = {0, 0};
                levelSelected = 13;
                break;
            }

            render_system.drawMenu(0);
            glfwPollEvents();
        }

        if (world_system.prev_state == MENU) {

            debugging.in_full_view_mode = false;
            menu_entities.clear();
            registry.remove_all_components_of(loadingScreen);
            registry.remove_all_components_of(ubzTitle);
            registry.remove_all_components_of(playButton);

            world_system.game_state = PLAYING;

            world_system.loadFromSave();
            if (isLevelSelected) {
                world_system.curr_level = levelSelected;
            }
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
                world_system.menu_click_pos = {0, 0};
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
                levelSelected = world_system.curr_level;
                world_system.menu_click_pos = {0, 0};
                break;
            }

            render_system.drawMenu(0);
        }
    }

	return EXIT_SUCCESS;
}
