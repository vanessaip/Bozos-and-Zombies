
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
	if (__cplusplus == 202002L) std::cout << "C++20\n";
	else if (__cplusplus == 201703L) std::cout << "C++17\n";
	else if (__cplusplus == 201402L) std::cout << "C++17\n";
	else if (__cplusplus == 201103L) std::cout << "C++11\n";
	else if (__cplusplus == 199711L) std::cout << "C++98\n";
	else std::cout << "None of the above\n";
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
	debugging.in_full_view_mode = true;
	// Entity loadingScreen = createLoadingScreen(&render_system, { window_width_px / 2, window_height_px / 2 }, { 2 * window_width_px, 3 * window_height_px });
  Entity loadingScreen = createAnimatedBackgroundObject(&render_system, { 728, 720 }, { 130, 130 }, TEXTURE_ASSET_ID::MM_FOUNTAIN, { 4 }, { 0, 0.01 });
	// variable timestep loop
	auto t = Clock::now();
	float total_elapsed = 0.f;
  while (total_elapsed < 4000.f) {
    glfwPollEvents();
    auto now = Clock::now();
		float elapsed_ms =
			((float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000) - world_system.pause_duration;
		total_elapsed += elapsed_ms;
		t = now;
    render_system.step(elapsed_ms);
    render_system.drawMenu(elapsed_ms);
  }

  world_system.init(&render_system);
	while (!world_system.is_over()) {
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

		// printf("total elapsed time: %f\n", total_elapsed);
		if (total_elapsed > 4000.f) {
			if(registry.motions.has(loadingScreen)) {
				debugging.in_full_view_mode = false;
				registry.remove_all_components_of(loadingScreen);
			}
			world_system.step(elapsed_ms);
			physics_system.step(elapsed_ms);
			render_system.step(elapsed_ms);
			world_system.handle_collisions();
		}

		render_system.draw(elapsed_ms);
	}

	return EXIT_SUCCESS;
}
