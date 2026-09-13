#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdbool.h>
#include <raylib.h>

#define TILE_SIZE 128 //ACTUAL TILE SIZE 128
#define MAP_ROWS 34
#define MAP_COLS 50
#define LEVEL_COUNT 3

// tile values: 0 = empty, 1 = solid wall, 2 = door/gate, 3 = spike

extern int maps[3][MAP_ROWS][MAP_COLS];
extern int currentLevel;

// Gate is always open for now -- no "all enemies dead" gating. Kept as a
// variable (rather than deleted) since CollisionX/CollisionY and the
// renderer already branch on it, in case a closable gate comes back later.
extern bool doorOpen;

// Where the player is placed after walking through the gate into a given
// level. Index with the level being ENTERED (i.e. levelSpawn[currentLevel]
// after currentLevel has already been advanced). These just drop the player
// into open air near the top of the arena and let gravity/CollisionY settle
// them onto the floor -- tune per level once the real layouts are finalized.
extern Vector2 levelSpawn[LEVEL_COUNT];

#endif