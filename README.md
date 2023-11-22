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
For milestone 2, our team improved the gameplay by designing a more complex map, introducing multiple zombies and NPCs, adding interactions between player/NPCs (acquiring textbook) and zombie/NPCs (infection), ability to kill zombies by aiming and throwing collected textbook projectiles, implemented heuristics for zombies chasing the player and humans, introduced food items to collect, and lives system.

We also hit all other requirements by adding camera movement that follows the player (with option to zoom in/out by pressing P), sprite animation for all our characters (player, zombies, NPCs), mesh collision, a tutorial, and plenty of new visual assets to support these features.

# Milestone 2 Deliverables
# Improved Gameplay

## Game Logic response to user input
- NPCs can die and then turn into zombies that follow the player, adding depth to the gameplay.
    - **Code References**:
      - Transformation logic in `world_system.cpp`: `step()` function on lines `413 - 421`; `526 - 560`.
      - Collision handling in `world_system.cpp`: `handle_collisions()` on lines `1181 - 1220`.

Ladder interaction:
- in world_system.cpp, Line 423 checks if the entity is a player and calls `updateClimbing()`
- Line 862 has the implementation for updateClimbing which loops through every Climbable entity and checks if the player is touching it. If the player is touching it, allow the player to use keys W and S to climb up and down the ladder
- Line 911 has a helper function `isBottomOfLadder()` to resolve glitches that arise when the player is at the bottom of the ladder but should not be able to move down

Zombie heuristic/decision based movement:
- in world_system.cpp, Line 469 checks if the entity is a zombie and calls `updateZombieMovement()`
- Line 638 has the implementation for updateZombieMovement and is based on the following heuristics:
1. If the player is on the same level as the player, move in the direction of the player.
2. If the player is above the zombie, have the zombie move to the closest ladder of the player and climb up.
3. If the player is below the zombie, have the zombie move to the closest ladder of the player and climb down.
4. If the player is in the basement, head to the basement ladder and climb down. 
5. If the player is out of the basement and the zombie is in the basement, head to the basement ladder and climb up.
6. If the zombie encounters a step, then jump over it.
- the helper function `checkLevel()` is implemented on Line 816
- the helper function `getClosestLadder()` is implemented on Line 841

Aiming Cursor: 
- in world_init.cpp, line 37 implements a createBozoPointer function that spawns a cursor that attaches to the player.
- in world_system.cpp line 1387 we implement the logic for the mouse to follow the cursor.

Help: Provide basic user tutorial/help.
- Using createStaticTexture() in world_unit.cpp, we spawn a sign that explains the basic controls of the game in the center of the stage.

Winning Screen:
- Using createStaticTexture() defined in world_unit.cpp, when all the zombies have been deleted and all foods have been collected, we zoom the camera out to the full map and display a "You Win!" message.


## Sprite sheet animation
- Sprite sheet component defined in components.hpp line 293
- initializeSpriteSheet creates a new sprite sheet component and binds vbo and ibo to unique buffer Id		
	- Defined in render_system.cpp line 413
	- Called in zombie, student, player create functions
- updateSpriteSheetGeometryBuffer updates the coordinates on the sprite sheet
	- Defined in render_system.cpp line 435
	- Called in RenderSystem::step line 260

## New integrated assets
- **Introduction of New Spike Mesh Assets**:
  - Spikes increase the game's challenge and visual appeal.
    - **Code References**:
      - Asset initialization in `world_system.cpp` within the `restart()` function.
      - Definitions and shaders in `components.hpp` (lines `42 - 45`), `spike.fs.glsl`, `spike.vs.glsl`, and `spike.obj`.

- new background assets initialized with `createBackground()`  
- floors are created with fixed sized platforms stacked horizontally using `createPlatforms()`
- steps are created with stacking platforms at an offset `createSteps()` 
- different length ladders created by stacking 3 different sprite assets `createClimbable()`

## Mesh-based collision detection
- **Enhanced Collision Detection Function**:
  - Improved function for interactions between player and mesh assets.
    - **Code Reference**:
      - `bool checkCollision` on lines `46 to 74` of physics_system.cpp.

## Basic user tutorial/help

# Playability

Food collection:
- in world_init.cpp, Line 410 has `createFood()` method 
- in world_system.cpp, Line 1056 creates the food entities 
- Line 1239 checks if the player has collided with the food and adds it to the overlay component

Player lives:
- in world_init.cpp, Line 410 has `createHeart()` method 
- in world_system.cpp, Line 1066 creates the hearts
- Line 1096 checks the player's lives, adds the player to the lostLifeTimer component, and moves the player back to the starting position
- Line 239 checks if the player is in the "lost life" state, dims the player, and makes the player invincible for a small amount of time

Overlay rendering for food/lives feedback:
- in render_system.cpp Line 238, the logic blocks splits the call to drawTexturedMesh between the side scroller projection matrix and the basic projection matrix depending on if the overlay component has the entity
- in world_system.cpp, Line 1241 adds the specific food entity to the overlay component, so that it can be rendered statically on top of the side scrolling image
- in world_init.cpp, the `createHeart()` method directly adds the heart to the overlay component

## 2 minutes of non-repeptitive gameplay

**Projectile system:**
- in world_system.cpp `handle_collisions()`, when player collides with human NPC, player receives a book and will hold it. The number of books received will be shwon on title.
- in world_system.cpp `on_mouse_button()`, player will launch the book when releasing the mouse left button, the direction is the same as the blue pointer

# Stability

## Minimal Lag

- all code

## Consistent game resolution

- world_system.cpp line 82: allow window to be resized
- render_system_init.cpp line 23: keep the aspect ratio consistent
- the viewport is resized on each call to `drawToScreen()` in render_system.cpp

## No crashes, glitches, unpredictable behaviour
**Bugs fixed including**:
- mouse not pointing at cursor after zoomed in, fixed by calculate relative position to the camera
- camera splits into 2 sometimes, fixed by changing the logic of timer
# Creative

## [21] Make the camera follow the player
- createProjectionMatrix determines the camera window dimensions based on player position
- defined in render_system.cpp line 265
- called in RenderSystem::draw line 229

## [24] Basic integrated assets
- new sprites for platforms, walls, and ladders
- (created) several sprites for background components (outdoor background, indoor background layers, nest egg)
- (created) blackboard sprite for user tutorial
- (created) book projectile
- 3 spritesheets for our characters
- 6 differnt sprites for collectable food items
- 1 new heart sprite for player lives
- (created) spike mesh object file
- (created) arrow sprite for projectile direction

# Project References

Level ending sound effect - pixabay.com
Collected item sound effect - pixabay.com
Tutorial background music - pixabay.com
Wreck Beach level background music - "Tahitian Remix Despacito" by Tulei 
To be Continue music - "Grim Grinning Ghosts (Electro Swing Mix)" by Glenn Gatsby and Ashley Slater

