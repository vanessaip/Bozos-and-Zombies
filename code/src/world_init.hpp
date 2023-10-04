#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float STUDENT_BB_WIDTH = 0.4f * 296.f;
const float STUDENT_BB_HEIGHT = 0.4f * 165.f;
const float ZOMBIE_BB_WIDTH = 0.4f * 400.f;
const float ZOMBIE_BB_HEIGHT = 0.4f * 400.f;
const float BOZO_BB_WIDTH = 0.4f * 400.f;
const float BOZO_BB_HEIGHT = 0.4f * 400.f;

// the player
Entity createBozo(RenderSystem* renderer, vec2 pos);
// the prey
Entity createStudent(RenderSystem* renderer, vec2 position);
// the enemy
Entity createZombie(RenderSystem* renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);


