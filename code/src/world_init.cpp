#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createBozo(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	
	// Setting initial values
	motion.scale = vec2({ BOZO_BB_WIDTH, BOZO_BB_HEIGHT });

	// Create and (empty) Bozo component to be able to refer to all players
	registry.players.emplace(entity);
	registry.humans.emplace(entity); // zombies will target all entities with human component

	std::vector<int> spriteCounts = { 4, 6, 6 };
	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::IDLE, spriteCounts, 100.f, vec2(0.05f, 0.1f));
	//registry.spriteSheets.emplace(entity, SpriteSheet(5, ANIMATION_MODE::IDLE, spriteCounts, 100.f, vec2(0.05f, 0.1f)));
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOZO,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE_SHEET });

	return entity;
}

Entity createStudent(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -STUDENT_BB_WIDTH, STUDENT_BB_HEIGHT });

	// Create an (empty) Student component to be able to refer to all students
	registry.humans.emplace(entity);

	std::vector<int> spriteCounts = { 4, 6, 6 };
	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::RUN, spriteCounts, 100.f, vec2(0.05f, 0.1f));

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::STUDENT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE_SHEET });

	return entity;
}

Entity createZombie(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -ZOMBIE_BB_WIDTH, ZOMBIE_BB_HEIGHT });

	// Create and (empty) Zombie component to be able to refer to all zombies
	registry.zombies.emplace(entity);
	std::vector<int> spriteCounts = { 8, 6, 5 };
	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::RUN, spriteCounts, 75.f, vec2(0.01f, 0.1f));
	//registry.spriteSheets.emplace(entity, SpriteSheet(7, ANIMATION_MODE::RUN, spriteCounts, 75.f, vec2(0.01f, 0.1f)));


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ZOMBIE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE_SHEET });

	return entity;
}

// Platform has variable width (possibly variable height in future)
Entity createPlatform(RenderSystem* renderer, vec2 position, float width)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ width, PLATFORM_HEIGHT });

	// Create a Platform component
	registry.platforms.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLATFORM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Wall has variable height
Entity createWall(RenderSystem* renderer, vec2 position, float height)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ WALL_WIDTH, height });

	// Create a Wall component
	registry.walls.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLATFORM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBackground(RenderSystem* renderer)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = { window_width_px/2, window_height_px/2 }; //TODO(vanessa): need to ensure all backgrounds are the same dimensions 

	// Setting initial values
	motion.scale = vec2({ window_width_px, window_height_px });

	// Create a Background component
	registry.backgrounds.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BACKGROUND,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT});

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}