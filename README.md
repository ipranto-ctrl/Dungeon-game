Dungeon Crawler

A 2D dungeon crawler game built in C using the raylib library. The game features 3 levels of increasing difficulty, multiple enemy types, environmental hazards, and a boss fight at the end.


Gameplay


Fight through 3 dungeon levels
Kill all enemies to open the door to the next level
Survive environmental hazards scattered throughout the dungeon
Face a boss in the final room



Controls

KeyActionA / DMove left / rightSpaceJump (double jump available)Left ShiftDashBackspaceAttack (direction based on facing)W + BackspaceAttack upwardEscapePauseEnterConfirm / ResumeDeleteQuit


Features

Player


Smooth movement with delta time (framerate independent)
Double jump
Dash with cooldown
Directional melee attack (left, right, up)
Health system with invincibility frames on hit
Invincibility flash effect


Enemies


Bull — charges at the player, stops and turns when it overshoots. Accelerates during charge. Variable turn timer and gravity per instance.
Spirit — homes in on the player, speeds up when the player moves. Explodes on contact with a knockback effect, then dies.
Mimic — chases the player and performs a melee attack when close. Has knockback on hit. Can be killed by the player's attack.
Archer (in progress) — ranged enemy that fires arrows at the player when in range, runs away when the player gets too close.


Environmental Hazards


Spike traps — tile-based hazards (tile value 3) that deal damage on contact with iframes. Knockback on hit.


Level System


3 levels stored as a 3D tilemap array
Tile values: 0 = empty, 1 = solid, 2 = door, 3 = spike
Door opens when all enemies in the level are dead
Camera lerps smoothly behind the player


UI


Health bar (background + foreground)
Dash cooldown display
Main menu, pause menu, game over screen with restart



File Structure

├── test.c          — main game loop, game states, entity initialization
├── player.h        — Player struct and function declarations
├── player.c        — movement, dash, jump, gravity, attack
├── collisionX.c    — player tile collision (X and Y axis, spike damage)
├── jump.c          — jump and gravity helpers
├── enemies.h       — all enemy structs and declarations
├── enemies.c       — Bull and Spirit logic
├── enemies2.c      — Mimic and Archer logic
├── tilemap.h       — tilemap constants and extern declarations
├── tilemap.c       — 3D map array and currentLevel


Build

Make sure raylib is installed, then:

bashgcc test.c player.c collisionX.c jump.c enemies.c enemies2.c tilemap.c \
    -o game -lraylib -lm -lpthread -ldl -lX11 && ./game

Or use the build script if included:

bashchmod +x run.sh
./run.sh


Team


Gameplay & Engine — core systems, physics, collision, enemy AI
Art & Sprites — textures replacing placeholder rectangles



Status

Work in progress. Currently implemented:


 Player movement, dash, double jump, attack
 Bull, Spirit, Mimic enemies
 Spike hazards
 3-level tilemap system
 Health, iframes, game states
 Archer projectile system
 Level transition (door logic)
 Sprites
 Boss fight
 Ability pickups
