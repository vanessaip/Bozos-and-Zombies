#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

using Clock = std::chrono::high_resolution_clock;

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

	std::vector<int> spriteCounts = { 4, 6, 6, 6 };
	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::IDLE, spriteCounts, 100.f, vec2(0.05f, 0.08f));
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


Entity createStudent(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId)
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
	registry.colors.insert(entity, { 1, 0.8f, 0.8f });

	std::vector<int> spriteCounts = { 4, 6, 6 };
	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::RUN, spriteCounts, 100.f, vec2(0.05f, 0.1f));

	registry.renderRequests.insert(
		entity,
		{ textureId,
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
	motion.velocity = { 100.f, 0.f };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -ZOMBIE_BB_WIDTH, ZOMBIE_BB_HEIGHT });

	motion.speedMultiplier = 1 + 0.1 * sin(rand());

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
Entity createPlatform(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture, bool visible, vec2 scale)
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
	if (visible == true) {
		registry.renderRequests.insert(
			entity,
			{ texture,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}

	return entity;
}

// creates a horizontal line of platforms starting at left_position from num_tiles repeated platform sprites
std::vector<Entity> createPlatforms(RenderSystem* renderer, float left_position_x, float left_position_y, uint num_tiles, TEXTURE_ASSET_ID texture, bool visible, vec2 scale)
{
	// TODO(vanessa): check platform dimensions in bounds
	std::vector<Entity> platforms;
	vec2 curr_pos = { left_position_x, left_position_y };
	for (uint i = 0; i < num_tiles; i++) {
		Entity p = createPlatform(renderer, curr_pos, texture, visible, scale);
		platforms.push_back(p);
		curr_pos.x += scale.x;
	}
	return platforms;
}

// TODO(vanessa): maybe make new component for steps, step blocks must be >0
std::vector<Entity> createSteps(RenderSystem* renderer, vec2 left_pos, vec2 step_scale, uint num_steps, uint step_blocks, bool left)
{
	std::vector<Entity> steps;
	vec2 curr_pos = left_pos;
	for (uint i = 0; i < num_steps; i++) {
		if (left) {
			Entity s0 = createPlatform(renderer, curr_pos, TEXTURE_ASSET_ID::STEP0, true, step_scale);
			Motion& m = registry.motions.get(s0);
			m.reflect.x = true; // TODO(vanessa): shouldn't it be reflect y?
			steps.push_back(s0);
			curr_pos.x += step_scale.x;
		}
		for (uint j = 0; j < step_blocks - 1; j++) {
			Entity s = createPlatform(renderer, curr_pos, TEXTURE_ASSET_ID::STEP1, true, step_scale);
			steps.push_back(s);
			curr_pos.x += step_scale.x;
		}
		if (!left) {
			Entity s0 = createPlatform(renderer, curr_pos, TEXTURE_ASSET_ID::STEP0, true, step_scale);
			steps.push_back(s0);
			curr_pos.y += step_scale.y;
		}
		else {
			curr_pos.x -= step_scale.x;
			curr_pos.y -= step_scale.y;
		}
	}
	return steps;
}

// Wall has variable height
Entity createWall(RenderSystem* renderer, float position_x, float position_y, float height, bool visible)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = { position_x, position_y };

	// Setting initial values
	motion.scale = vec2({ WALL_WIDTH, height });

	// Create a Wall component
	registry.walls.emplace(entity);
	if (visible == true) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::WALL,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	return entity;
}

std::vector<Entity> createClimbable(RenderSystem* renderer, float top_position_x, float top_position_y, uint num_sections, TEXTURE_ASSET_ID texture)
{
	std::vector<Entity> sections;
	for (uint i = 0; i < num_sections; i++) {

		auto entity = Entity();
		sections.push_back(entity);

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Initialize the motion
		auto& motion = registry.motions.emplace(entity);
		motion.angle = 0.f;
		motion.velocity = { 0.f, 0.f };
		motion.position = { top_position_x, top_position_y };
		top_position_y += CLIMBABLE_DIM.y;

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

Entity createBackground(RenderSystem* renderer, TEXTURE_ASSET_ID texture, float depth, vec2 position, vec2 scale)
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
	Background& background = registry.backgrounds.emplace(entity);
	background.depth = depth;

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
	Bounce &bounce = registry.bounce.emplace(entity);
	bounce.mass = std::numeric_limits<int>::max();

	registry.spikes.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::SPIKE,
			GEOMETRY_BUFFER_ID::SPIKE });

	return entity;
}

Entity createWheel(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::WHEEL);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 15.f;
	motion.offGround = true;

	registry.wheels.emplace(entity);
	Bounce &bounce = registry.bounce.emplace(entity);
	bounce.mass = 100.0f;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::WHEEL,
			GEOMETRY_BUFFER_ID::WHEEL });

	return entity;
}

Entity createBook(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID textureId)
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
		{ textureId,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createStaticTexture(RenderSystem* renderer, TEXTURE_ASSET_ID textureID, vec2 position, std::string text, vec2 scale)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& textbox = registry.textboxes.emplace(entity);
	textbox.text = text;

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	// Create an (empty) Book component to be able to refer to all books
	registry.renderRequests.insert(
		entity,
		{ textureID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createCollectible(RenderSystem* renderer, float position_x, float position_y, TEXTURE_ASSET_ID collectible, vec2 scale, bool overlay = false)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = { position_x, position_y };
	motion.scale = scale;

	if (overlay) {
		registry.overlay.emplace(entity);
	}
	else {
		registry.collectible.emplace(entity);
		registry.collectible.get(entity).collectible_id = (int)collectible;
	}
	registry.renderRequests.insert(
		entity,
		{ collectible,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHeart(RenderSystem* renderer, vec2 position, vec2 scale) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	registry.overlay.emplace(entity);


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEART,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createDangerous(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, vec2 p0, vec2 p1, vec2 p2, vec2 p3, bool cubic) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	Dangerous& dangerous = registry.dangerous.emplace(entity);

  dangerous.p0 = p0;
  dangerous.p1 = p1;
  dangerous.p2 = p2;
  dangerous.p3 = p3;
  dangerous.cubic = cubic;

	registry.renderRequests.insert(
		entity,
		{ assetID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLoadingScreen(RenderSystem* renderer, vec2 position, vec2 scale) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;
	
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LOADING_SCREEN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}
Entity createOverlay(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, bool is_fading) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	if(is_fading) {
		registry.fading.emplace(entity);
		Fading& fading = registry.fading.get(entity);
		fading.fading_timer = Clock::now();
	}

	registry.overlay.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ assetID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createDoor(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	auto& door = registry.doors.emplace(entity);

	std::vector<int> spriteCounts = { 1,6,1 }; // one frame closed, 6 frames animate open, 1 frame closed
	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::IDLE, spriteCounts, 400.f, vec2(0.f, 0.0f));

	registry.renderRequests.insert(
		entity,
		{ assetID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE_SHEET });
	return entity;
}

Entity createBoss(RenderSystem* renderer, vec2 position, vec2 scale, float health, float damage, TEXTURE_ASSET_ID assetID, vec2 trunc, std::vector<int> counts) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	Boss& boss = registry.bosses.emplace(entity);

  registry.colors.insert(entity, { 1, 1, 1 });

  boss.health = health;
  boss.damage = damage;

  std::vector<int> spriteCounts = counts;
	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::RUN, spriteCounts, 100.f, trunc);

	registry.renderRequests.insert(
		entity,
		{ assetID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE_SHEET });

	return entity;
}

Entity createHPBar(RenderSystem* renderer, vec2 position) {
  // Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = {80, 10};

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HP_BAR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHP(RenderSystem* renderer, vec2 position) {
  // Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = {80, 10};

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HP,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createAnimatedBackgroundObject(RenderSystem* renderer, vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID, std::vector<int> spriteCounts, vec2 trunc) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = scale;

	renderer->initializeSpriteSheet(entity, ANIMATION_MODE::IDLE, spriteCounts, 100.f, trunc);

	registry.renderRequests.insert(
		entity,
		{ assetID,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE_SHEET });
	return entity;
}

// Wrapper method for removing entities
// Removes all entity components and resets buffer id for sprite sheet components
void removeEntity(Entity e) {
	if (registry.spriteSheets.has(e))
	{
		SpriteSheet& spriteSheet = registry.spriteSheets.get(e);
		RenderSystem::deleteBufferId(static_cast<int>(spriteSheet.bufferId));
	}

	registry.remove_all_components_of(e);
}