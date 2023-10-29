#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float STUDENT_BB_WIDTH = 30.f;
const float STUDENT_BB_HEIGHT = 50.f;
const float ZOMBIE_BB_WIDTH = 20.f;
const float ZOMBIE_BB_HEIGHT = STUDENT_BB_HEIGHT;
const float BOZO_BB_WIDTH = STUDENT_BB_WIDTH;
const float BOZO_BB_HEIGHT = STUDENT_BB_HEIGHT;
const float PLATFORM_HEIGHT = 30.f;
const float PLATFORM_WIDTH = 50.f;
const float WALL_WIDTH = 20.f;
const float STEP_HEIGHT = 30.f;
const float STEP_WIDTH = 30.f;
const vec2 CLIMBABLE_DIM = {22.f, 32.f};

// the player
Entity createBozo(RenderSystem* renderer, vec2 pos);
// the prey
Entity createStudent(RenderSystem* renderer, vec2 position);
// the enemy
Entity createZombie(RenderSystem* renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// one platform
Entity createPlatform(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture, vec2 scale = { PLATFORM_WIDTH, PLATFORM_HEIGHT });
// helper for multiple platforms lined up
std::vector<Entity> createPlatforms(RenderSystem* renderer, vec2 left_position, uint num_tiles);
// helper for steps
std::vector<Entity> createSteps(RenderSystem* renderer, vec2 left_pos, uint num_steps, uint step_blocks, bool left);
// walls
Entity createWall(RenderSystem* renderer, vec2 position, float height);
// ladders
std::vector<Entity> createClimbable(RenderSystem* renderer, vec2 top_position, uint num_sections);
// spikes
Entity createSpike(RenderSystem* renderer, vec2 pos);
// background
Entity createBackground(RenderSystem* renderer, TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::BACKGROUND, vec2 position = { window_width_px/2, window_height_px/2 }, vec2 scale = { window_width_px, window_height_px });


