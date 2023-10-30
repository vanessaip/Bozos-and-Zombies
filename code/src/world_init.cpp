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
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOZO,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE_SHEET });

	return entity;
}


Entity createBozoPointer(RenderSystem* renderer, vec2 pos) 
{	
	
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	// motion.position = player_motion.position;
	motion.position = pos;
	motion.angle = 0.f;
	// motion.velocity = player_motion.velocity;
	motion.velocity = { 0.f, 0.f };

	// Setting initial values
	motion.scale = vec2({ BOZO_POINTER_BB_WIDTH, BOZO_POINTER_BB_HEIGHT });

	registry.playerEffects.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOZO_POINTER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

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

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ZOMBIE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE_SHEET });

	return entity;
}

// Platform has variable width (possibly variable height in future)
Entity createPlatform(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture, vec2 scale)
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
	motion.scale = scale;

	// Create a Platform component
	registry.platforms.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// creates a horizontal line of platforms starting at left_position from num_tiles repeated platform sprites
std::vector<Entity> createPlatforms(RenderSystem* renderer, vec2 left_position, uint num_tiles)
{
	// TODO(vanessa): check platform dimensions in bounds
	std::vector<Entity> platforms;
	vec2 curr_pos = left_position;
	for (uint i = 0; i < num_tiles; i++) {
		Entity p = createPlatform(renderer, curr_pos, TEXTURE_ASSET_ID::STEP1);
		platforms.push_back(p);
		curr_pos.x += PLATFORM_WIDTH;
	}
	return platforms;
}

// TODO(vanessa): maybe make new component for steps, step blocks must be >0
std::vector<Entity> createSteps(RenderSystem* renderer, vec2 left_pos, uint num_steps, uint step_blocks, bool left)
{
	std::vector<Entity> steps;
	vec2 curr_pos = left_pos;
	for (uint i = 0; i < num_steps; i++) {
		if (left) {
			Entity s0 = createPlatform(renderer, curr_pos, TEXTURE_ASSET_ID::STEP0, { STEP_WIDTH, STEP_HEIGHT });
			Motion& m = registry.motions.get(s0);
			m.reflect.x = true; // TODO(vanessa): shouldn't it be reflect y?
			steps.push_back(s0);
			curr_pos.x += STEP_WIDTH;
		}
		for (uint j = 0; j < step_blocks - 1; j++) {
			Entity s = createPlatform(renderer, curr_pos, TEXTURE_ASSET_ID::STEP1, { STEP_WIDTH, STEP_HEIGHT });
			steps.push_back(s);
			curr_pos.x += STEP_WIDTH;
		}
		if (!left) {
			Entity s0 = createPlatform(renderer, curr_pos, TEXTURE_ASSET_ID::STEP0, { STEP_WIDTH, STEP_HEIGHT });
			steps.push_back(s0);
			curr_pos.y += STEP_HEIGHT;
		}
		else {
			curr_pos.x -= STEP_WIDTH;
			curr_pos.y -= STEP_HEIGHT;
		}
	}
	return steps;
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
		{ TEXTURE_ASSET_ID::WALL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

std::vector<Entity> createClimbable(RenderSystem* renderer, vec2 top_position, uint num_sections)
{
	std::vector<Entity> sections;
	TEXTURE_ASSET_ID texture;
	for (uint i = 0; i < num_sections; i++) {
		if (i == 0) {
			texture = TEXTURE_ASSET_ID::LADDER1;
		}
		else if (i == num_sections - 1) {
			texture = TEXTURE_ASSET_ID::LADDER3;
		}
		else {
			texture = TEXTURE_ASSET_ID::LADDER2;
		}

		auto entity = Entity();
		sections.push_back(entity);

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Initialize the motion
		auto& motion = registry.motions.emplace(entity);
		motion.angle = 0.f;
		motion.velocity = { 0.f, 0.f };
		motion.position = top_position;
		top_position.y += CLIMBABLE_DIM.y;

		motion.scale = CLIMBABLE_DIM;

		registry.climbables.emplace(entity);
		registry.renderRequests.insert(
			entity,
			{ texture,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	}

	return sections;
}

Entity createBackground(RenderSystem* renderer, TEXTURE_ASSET_ID texture, vec2 position, vec2 scale)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position; //TODO(vanessa): need to ensure all backgrounds are the same dimensions 

	// Setting initial values
	motion.scale = scale;

	// Create a Background component
	registry.backgrounds.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ texture,
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
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createSpike(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPIKE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 25.f;
	motion.scale.y *= -1.25;

	registry.spikes.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::SPIKE,
			GEOMETRY_BUFFER_ID::SPIKE });

	return entity;
}

Entity createBook(RenderSystem* renderer, vec2 position)
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

	// Setting initial values, scale the same with player
	motion.scale = vec2({ 102.f * 0.3, 84.f * 0.3 });

	// Create an (empty) Book component to be able to refer to all books
	registry.books.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOOK,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}