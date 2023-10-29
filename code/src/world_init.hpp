#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float STUDENT_BB_WIDTH = 100.f;
const float STUDENT_BB_HEIGHT = 160.f;
const float ZOMBIE_BB_WIDTH = 250.f;
const float ZOMBIE_BB_HEIGHT = 200.f;
const float BOZO_BB_WIDTH = 120.f;
const float BOZO_BB_HEIGHT = 160.f;
const float PLATFORM_HEIGHT = 20.f;
const float WALL_WIDTH = 20.f;

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
// walls
Entity createWall(RenderSystem* renderer, vec2 position, float height);
// render background as a sprite
Entity createBackground(RenderSystem* renderer);


