#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float STUDENT_BB_WIDTH = 0.4f * 400.f;
const float STUDENT_BB_HEIGHT = 0.4f * 400.f;
const float ZOMBIE_BB_WIDTH = 0.4f * 400.f;
const float ZOMBIE_BB_HEIGHT = 0.4f * 400.f;
const float BOZO_BB_WIDTH = 0.4f * 400.f;
const float BOZO_BB_HEIGHT = 0.4f * 400.f;
const float PLATFORM_HEIGHT = 20.f;

// the player
Entity createBozo(RenderSystem* renderer, vec2 pos);
// the prey
Entity createStudent(RenderSystem* renderer, vec2 position);
// the enemy
Entity createZombie(RenderSystem* renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// platforms
Entity createPlatform(RenderSystem* renderer, vec2 position, float width);
// render background as a sprite
Entity createBackground(RenderSystem* renderer);


