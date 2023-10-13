# Milestone 1 Proposal Alignment

# Milestone 1 Deliverables

# Rendering

## Textured Geometry

- walls, platforms, player sprite, npc sprite, zombie sprite
- Entry point:

## Basic 2D Transformations

- scale, rotate, translate, reflect
- Entry point: common.cpp Line 4

## Key-frame/State Interpolations

-

# Gameplay

## Keyboard/Mouse Control

- A key moves player left, D key moves player right, W key makes player jump
  <br>
  <br>
  Entry point:
- world_system.pcp Line 529

## Random/coded action

## Well-defined game-space boundaries

## Correct collision processing

- player - platform/wall collision, player - zombie collision, player - npc collision
  <br>
  <br>
  Entry points:
- platform collisions are implemented in a for loop in world_system.cpp 176 to 237 which uses bounding boxes to check every platform for intersection with the player entity's bounding box.

# Stability

## Minimal Lag

- all code

## No crashes, glitches, unpredictable behaviour

- all code

# Creative

## [8] Basic Physics

- gravity acts on all human entities when not standing on a platform/ground
  <br>
  <br>
  Entry points:
- added offGround member to Motion struct in components.hpp
- physics_system.cpp Line 57 adds gravity velocity if the entity is a human and motion param offGround == true
- world_system.cpp Line 191 detects the intersection of the bottom of the player's bounding box with the top of the platform's bounding box and sets the offGround member to false. Changes back to true once bottom of the player's bounding box is no longer in the plane of any platform's top bounding box.

## [23] Audio Feedback
