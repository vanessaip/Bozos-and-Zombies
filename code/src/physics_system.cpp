// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

// float GRAVITY = 10;

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion &motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return {abs(motion.scale.x), abs(motion.scale.y)};
}

glm::vec2 calculateNormalizedProjectionAxis(const glm::vec2 &current, const glm::vec2 &next)
{
	glm::vec2 edge = next - current;
	glm::vec2 normal = glm::vec2(-edge.y, edge.x);
	return glm::normalize(normal);
}

void computeProjections(const std::vector<glm::vec2> &vertices, const glm::vec2 &axis, std::vector<double> &projections)
{
	projections.clear();
	for (const auto &vertex : vertices)
	{
		projections.push_back(glm::dot(axis, vertex));
	}
}

bool isOverlapping(const std::vector<double> &projections1, const std::vector<double> &projections2)
{
	auto minProj1 = *std::min_element(projections1.begin(), projections1.end());
	auto maxProj1 = *std::max_element(projections1.begin(), projections1.end());

	auto minProj2 = *std::min_element(projections2.begin(), projections2.end());
	auto maxProj2 = *std::max_element(projections2.begin(), projections2.end());

	return !(maxProj1 < minProj2 || maxProj2 < minProj1);
}

bool isPointInsidePolygon(const glm::vec2& point, const std::vector<glm::vec2>& polygon) {
    bool inside = false;
    for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
            (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

bool checkSATIntersection(const std::vector<glm::vec2> &vertices1, const std::vector<glm::vec2> &vertices2)
{
	std::vector<double> projections1, projections2;

	for (size_t i = 0; i < vertices1.size(); ++i)
	{
		glm::vec2 axis = calculateNormalizedProjectionAxis(vertices1[i], vertices1[(i + 1) % vertices1.size()]);
		computeProjections(vertices1, axis, projections1);
		computeProjections(vertices2, axis, projections2);

		if (!isOverlapping(projections1, projections2))
		{
			return false;
		}
	}

	for (size_t i = 0; i < vertices2.size(); ++i)
	{
		glm::vec2 axis = calculateNormalizedProjectionAxis(vertices2[i], vertices2[(i + 1) % vertices2.size()]);
		computeProjections(vertices1, axis, projections1);
		computeProjections(vertices2, axis, projections2);

		if (!isOverlapping(projections1, projections2))
		{
			return false;
		}
	}

// Check containment
    for (const auto& vertex : vertices1) {
        if (isPointInsidePolygon(vertex, vertices2)) {
            return true;
        }
    }
    for (const auto& vertex : vertices2) {
        if (isPointInsidePolygon(vertex, vertices1)) {
            return true;
        }
    }

    return true;
	}

std::vector<glm::vec2> getTransformedVertices(const Mesh *mesh, const Motion &motion)
{
	std::vector<glm::vec2> transformedVertices;

	if (mesh == nullptr)
	{
		return transformedVertices;
	}

	for (const auto &vertex : mesh->vertices)
	{
		glm::vec3 pos = vertex.position;

		// Apply scale and translation
		glm::vec2 scaledPos = glm::vec2(pos.x * motion.scale.x, pos.y * motion.scale.y);
		glm::vec2 worldPos = scaledPos + glm::vec2(motion.position.x, motion.position.y);

		if (motion.angle != 0.0f)
		{
			float s = sin(motion.angle);
			float c = cos(motion.angle);

			// Rotate around the center of the entity
			worldPos = glm::vec2(
				worldPos.x * c - worldPos.y * s,
				worldPos.x * s + worldPos.y * c);
		}

		transformedVertices.push_back(worldPos);
	}

	return transformedVertices;
}

void resolve_bounce_collision(Entity entity1, Entity entity2)
{
	Motion &motion1 = registry.motions.get(entity1);
	Motion &motion2 = registry.motions.get(entity2);
	float &mass1 = registry.bounce.get(entity1).mass;
	float &mass2 = registry.bounce.get(entity2).mass;

	vec2 collisionNormal = normalize(motion2.position - motion1.position);
	vec2 relVelocity = motion2.velocity - motion1.velocity;
	float normalVelocity = dot(relVelocity, collisionNormal);
	if (normalVelocity > 0)
		return;

	float impact = -(1 + 0.5) * normalVelocity / (1 / mass1 + 1 / mass2); // 0.5 because I wanted a realistic collision
	vec2 impulse = impact * collisionNormal;
	if (registry.wheels.has(entity1))
	{
		motion1.velocity.x -= impulse.x / mass1;
		motion1.position.x -= 20.f;
	}
	else
	{
		motion2.velocity.x += impulse.x / mass2;
		motion2.position.x += 20.f;
	}
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion &motion1, const Motion &motion2)
{

	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp, dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

// void special_collision(const Entity& entity_bozo, const Entity& entity_plat) {
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
// }

bool checkCollision(const Motion &player, const Mesh *spikeMesh, const Motion &spike)
{
	if (spikeMesh == nullptr)
	{
		return false; // No mesh to check against
	}

	// Calculate the player's bounding box edges
	vec2 bb_half = get_bounding_box(player) / 2.f;
	float leftP = player.position.x - bb_half.x;
	float rightP = player.position.x + bb_half.x;
	float topP = player.position.y - bb_half.y;
	float bottomP = player.position.y + bb_half.y;

	float leftS = std::numeric_limits<float>::max();
	float rightS = std::numeric_limits<float>::lowest();
	float topS = std::numeric_limits<float>::max();
	float bottomS = std::numeric_limits<float>::lowest();

	// Check each vertex of the spike mesh
	for (const ColoredVertex &v : spikeMesh->vertices)
	{
		// Assume vertex position is in local space, transform to world space
		vec2 worldPos = spike.position + vec2(v.position.x, v.position.y) * spikeMesh->original_size * 25.f;
		leftS = std::min(leftS, worldPos.x);
		rightS = std::max(rightS, worldPos.x);
		topS = std::min(topS, worldPos.y);
		bottomS = std::max(bottomS, worldPos.y);

		// If any vertex is inside the player's bounding box, there's a collision
		if (worldPos.x >= leftP && worldPos.x <= rightP && worldPos.y >= topP && worldPos.y <= bottomP)
		{
			return true;
		}
	}

	// Check if the player's bounding box is entirely within the spike's mesh
	if (leftP >= leftS && rightP <= rightS && topP >= topS && bottomP <= bottomS)
	{
		return true;
	}

	return false; // No collision if no vertices are inside the bounding box
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto &motion_container = registry.motions;
	auto &meshPtr_container = registry.meshPtrs;

	for (uint i = 0; i < motion_container.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion &motion = motion_container.components[i];
		Entity entity = motion_container.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		if ((registry.players.has(entity)))
		{
			if (motion.velocity.x > 0)
				motion.reflect.x = false;
			else if (motion.velocity.x < 0)
				motion.reflect.x = true;
		}
		if ((registry.humans.has(entity) || registry.zombies.has(entity) || registry.books.has(entity) || registry.wheels.has(entity) || registry.bosses.has(entity) || registry.buses.has(entity)) && motion.offGround)
		{
			motion.velocity[1] += PhysicsSystem::GRAVITY * elapsed_ms / 1000.f;
		}

		// Step the spikeballs as per Bezier curves
		// Bezier curve equations from https://en.wikipedia.org/wiki/B%C3%A9zier_curve
		if (registry.dangerous.has(entity))
		{

			Dangerous &dangerous = registry.dangerous.get(entity);

			if (dangerous.bezier) {
				vec2 p0 = dangerous.p0;
				vec2 p1 = dangerous.p1;
				vec2 p2 = dangerous.p2;
				vec2 p3 = dangerous.p3;

				if (dangerous.bezier_time < 2000)
				{

				float t = dangerous.bezier_time / 1000;

				vec2 L0 = (1 - t) * p0 + t * p1;
				vec2 L1 = (1 - t) * p1 + t * p2;

				vec2 Q0 = (1 - t) * L0 + t * L1;

				if (!dangerous.cubic)
				{
					motion.position = Q0;
					dangerous.bezier_time += 10;
				}
				else
				{
					vec2 L2 = (1 - t) * p2 + t * p3;

					vec2 Q1 = (1 - t) * L1 + t * L2;

					vec2 C0 = (1 - t) * Q0 + t * Q1;

					motion.position = C0;
					dangerous.bezier_time += 4;
				}
				}
				else if (dangerous.bezier_time > 4000)
				{
				dangerous.bezier_time = 0;
				motion.position = p0;
				}
				else
				{
				dangerous.bezier_time += 10;
				}
			} else {
				motion.position[1] += 300 * elapsed_ms / 1000.f;
      		}
		}

		motion.position[0] += motion.velocity[0] * step_seconds;
		motion.position[1] += motion.velocity[1] * step_seconds;
	}

	// Check for collisions between all moving entities
	for (uint i = 0; i < motion_container.components.size(); i++)
	{	
		Motion &motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		if (registry.platforms.has(entity_i) || registry.walls.has(entity_i)) { // hack to optimize, block collisions handled in world_system
			continue;
		}

		Mesh *mesh_i = meshPtr_container.get(entity_i); // Get the second mesh

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < motion_container.components.size(); j++)
		{
			Motion &motion_j = motion_container.components[j];
			Entity entity_j = motion_container.entities[j];

			if (registry.platforms.has(entity_j) || registry.walls.has(entity_j)) { // hack to optimize, block collisions handled in world_system
				continue;
			}

			Mesh *mesh_j = meshPtr_container.get(entity_j); // Get the second mesh

			if ((registry.players.has(entity_i) && registry.spikes.has(entity_j)) || (registry.players.has(entity_j) && registry.spikes.has(entity_i)))
			{
				if (checkCollision(motion_i, mesh_j, motion_j) || checkCollision(motion_j, mesh_i, motion_i))
				{
					// Create a collisions event
					// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
					registry.collisions.emplace_with_duplicates(entity_i, entity_j);
					registry.collisions.emplace_with_duplicates(entity_j, entity_i);
				}
			}
			else if ((registry.wheels.has(entity_i) && registry.spikes.has(entity_j)) || (registry.wheels.has(entity_j) && registry.spikes.has(entity_i)))
			{

				std::vector<glm::vec2> transformedVertices1 = getTransformedVertices(mesh_i, motion_i);
				std::vector<glm::vec2> transformedVertices2 = getTransformedVertices(mesh_j, motion_j);

				if (checkSATIntersection(transformedVertices1, transformedVertices2) || checkSATIntersection(transformedVertices2, transformedVertices1))
				{
					resolve_bounce_collision(entity_i, entity_j);
				}
			}
			else
			{
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
}