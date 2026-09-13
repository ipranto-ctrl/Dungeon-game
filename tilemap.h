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

// Computes where the player should appear when entering `level` (whether
// via a gate or a fresh game start). Scans the tilemap for the bottom-left-
// most tile that is open, has open space above it (so the player fits),
// and has solid ground directly beneath it (not a spike) -- i.e. the
// bottom-left-most safe standing tile in the level. This replaces the old
// hardcoded per-level spawn points, which were eyeballed and ended up
// placing the player in the top-left corner instead of the bottom-left.
Vector2 GetLevelBottomLeftSpawn(int level);

#endif