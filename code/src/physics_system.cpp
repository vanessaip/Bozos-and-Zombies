// internal
#include "physics_system.hpp"
#include "world_init.hpp"

//float GRAVITY = 10;

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

//void special_collision(const Entity& entity_bozo, const Entity& entity_plat) {
//	Motion& motion_bozo = registry.motions.get(entity_bozo);
//	Motion& motion_plat = registry.motions.get(entity_plat);
//			float xPlatLeftBound = motion_plat.position.x - motion_plat.scale[0] / 2.f;
//			float xPlatRightBound = motion_plat.position.x + motion_plat.scale[0] / 2.f;
//			float yPlatPos = motion_plat.position.y - 85.f;
//			// Stand on platform
//		if (motion_bozo.velocity.y >= 0.f && motion_bozo.position.y <= yPlatPos && motion_bozo.position.y >= yPlatPos - STUDENT_BB_HEIGHT &&
//			motion_bozo.position.x > xPlatLeftBound && motion_bozo.position.x < xPlatRightBound) {
//			registry.collisions.emplace_with_duplicates(entity_bozo, entity_plat);
//		}
//}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_container = registry.motions;
	for(uint i = 0; i < motion_container.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_container.components[i];
		Entity entity = motion_container.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		if (registry.humans.has(entity) && motion.offGround) {
			motion.velocity[1] += PhysicsSystem::GRAVITY;
		}

		if (registry.players.has(entity) && !registry.deathTimers.has(entity)) {
			Player& player = registry.players.get(entity);
			motion.velocity[0] = 0;
			motion.velocity[1] = 0;

			if (player.keyPresses[0]) {
				motion.velocity[0] -= 400;
			}
			if (player.keyPresses[1]) {
				motion.velocity[0] += 400;
			}
		}

		motion.position[0] += motion.velocity[0] * step_seconds;
		motion.position[1] += motion.velocity[1] * step_seconds;
	}

	// Check for collisions between all moving entities
	for(uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			Entity entity_j = motion_container.entities[j];
			//if (registry.players.has(entity_i) && registry.platforms.has(entity_j)) {
			//	special_collision(entity_i,entity_j);
			//}
			if (collides(motion_i, motion_j))
			{				
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}
}