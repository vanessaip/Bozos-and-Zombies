# Milestone 1 Proposal Alignment
For milestone 1, our team met all our goals that aligned with the milestone deliverable requirements. Extra work beyond the scope that we specified in the proposal was not delivered, specifically desigining one level and introducing a simple throwable object. As these features are not required until Milestone 2, we decided to move the work forward and address them in the coming weeks.
# Milestone 1 Deliverables
# Rendering

## Textured Geometry

- walls, platforms, player sprite, npc sprite, zombie sprite, background (each are entities rendered with sprite textures)
Entry points:
- added new sprite textures under data/textures/ for each entity type
- components.hpp: 
  - defined (empty) Components for Player, Zombie, Human, Platform, Wall, Background
  - updated TEXTURE_ASSET_ID enum to include new assets
- render_system.hpp: updated texture_paths with file names of new textures
- tiny_ecs_registry.hpp: added `ComponentContainer` for each new type to the registry -X
- world_init.hpp: defined dimensions for each sprite
- world_init.cpp: defined functions to initialize each Entity type with the appropriate Components and appropriate constants for `RenderRequest` 
- world_system.cpp: calls initializer for each Entity type in `restart()` using painter's algorithm (rendering background entities first) and with appropriate starting position.

## Basic 2D Transformations

- scale, rotate, translate, reflect
- Entry point: common.cpp Line 4

## Key-frame/State Interpolations
- in components.hpp line 100, we define a KeyframeAnimation component that stores frames, timer info, etc.
- in world_system.cpp line 602, we define a function to setup motion frames for entities
- in world_system.cpp line 323, we interpolate between frames over time for each entity that has a KeyframeAnimation component

# Gameplay

## Keyboard/Mouse Control

- A key moves player left, D key moves player right, W key makes player jump
  <br>
  <br>
  Entry point:
- world_system.pcp Line 529

## Random/coded action

  NPC: 
  - The student spawns at a random location on the screen and a constant velocity towards left or right (world_system.cpp 431 - 437).
  - Once the student collides with the left or right wall, it switches direction. (world_system.cpp 257 - 262).
 
  Zombie: 
  - The zombie spawns on the right side of the screen at a random height with a constant velocity (world_system.cpp 424 - 428).
  - The zombie always moves towards and follows Bozo's position (world_system.cpp 271 - 282)

## Well-defined game-space boundaries
- Restrict window boundaries that make sure that the main player (Bozo) doesn't exit the game space (world_system.cpp 158 - 172)
- Restrict window boundaries that make sure that the NPC doesn't exit the game space (world_system.cpp 239 - 255)
- Remove any non moving entities when they exit the boundaries. This logic will be used as we move the camera with the player (world_system.cpp 263-268).


## Correct collision processing

- player - platform/wall collision, player - zombie collision, player - npc collision
  <br>
  <br>
  Entry points:
- platform collisions are implemented in a for loop in world_system.cpp 176 to 237 which uses bounding boxes to check every platform for intersection with the player entity's bounding box.
- wall collisions are implemented in a for loop in world_system.cpp 239 to 252 which pushes the player 5 pixels back when collides with the wall.
- when player collides with zombie, there will be a little jump animation and timer will be set in world_system.cpp line 456 to 482, player will rotate based on the direction of the collision happens in world_system.cpp line 292 to 309.
- npc collisions are the template in world_system.cpp line 486 to 491.

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
- in world_system.cpp line 100-113, we initialize and load the WAV audio files, and do some basic error catching.
- in world_system.cpp line 23-33, we destroy the audio components in the WorldSystem destructor.
- in world_system.cpp line 200, we detect when the player has just landed on the top of a playform, and play a landing noise
- in world_system.cpp line 550, we play a jump sound the the player jumps
- in world_system.cpp line 481, we play a death sound as the play is killed.

# Milestone 2 Proposal Alignment
TODO

# Milestone 2 Deliverables
# Improved Gameplay

## Game Logic response to user input


## Sprite sheet animation


## New integrated assets

## Mesh-based collision detection

## Basic user tutorial/help

# Playability

## 2 minutes of non-repeptitive gameplay

# Stability

## Minimal Lag

- all code

## Consistent game resolution

## No crashes, glitches, unpredictable behaviour

# Creative

## [21] Make the camera follow the player

## ??
- complex assets?
- mouse gestures
