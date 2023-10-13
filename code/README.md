# Milestone 1 Proposal Alignment
For milestone 1, our team met all our goals that aligned with the milestone deliverable requirements. Extra work beyond the scope that we specified in the proposal was not delivered, specifically desigining one level and introducing a simple throwable object. As these features are not required until Milestone 2, we decided to move the work forward and address them in the coming weeks.
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
- NPC: 
  The student spawns at a random location on the screen and a constant velocity towards left or right (world_system.cpp 431 - 437).
  Once the student collides with the left or right wall, it switches direction. (world_system.cpp 257 - 262).
- Zombie: 
  The zombie spawns on the right side of the screen at a random height with a constant velocity (world_system.cpp 424 - 428).
  The zombie always moves towards and follows Bozo's position (world_system.cpp 271 - 282)

## Well-defined game-space boundaries
- Restrict window boundaries that make sure that the main player (Bozo) doesn't exit the game space (world_system.cpp 158 - 172)
- Restrict window boundaries that make sure that the NPC doesn't exit the game space (world_system.cpp 239 - 255)
- Remove any non moving entities when they exit the boundaries. This logic will be used as we move the camera with the player (world_system.cpp 263-268).


## Correct collision processing

- player - platform/wall collision, player - zombie collision, player - npc collision
  <br>
  <br>
  Entry points:
- Player - Platform/Wall: platform collisions are implemented in a for loop in world_system.cpp 176 to 237 which uses bounding boxes to check every platform for intersection with the player entity's bounding box(world_system.cpp line176-254).

- Player - zombie: When player collides with zombie, there will be a little jump animation and timer will be set(world_system.cpp line456-482), player will rotate based on the direction of the collision happens(world_system.cpp line292-309).

- Player - npc: The template(world_system.cpp line486-491).

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
