#ifndef TILEMAP_H
#define TILEMAP_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    TILE_NONE = 0,
    TILE_GROUND,
    TILE_WALL,
    TILE_BORDER,
    TILE_COLLUSION
} TileType;

typedef struct {
    int id;
    TileType type;
} TileProperties;

typedef struct {
    Texture2D texture;
    char *texturePath;
    int firstGid;
    TileProperties *properties;
    int propertyCount;
} Tileset;

typedef struct {
    unsigned int *data;
    int width;
    int height;
} TileLayer;

typedef struct {
    // Map dimensions
    int width;
    int height;
    int tileWidth;
    int tileHeight;

    // Tileset data
    Tileset *tilesets;
    int tilesetCount;

    // Layer data
    TileLayer *layers;
    int layerCount;
} Tilemap;


Tilemap LoadTilemapBinary(const char *jsonPath);
void UnloadTilemap(Tilemap *tilemap);

void DrawTilemap(const Tilemap *tilemap);

bool IsTileWalkable(const Tilemap *tilemap, int x, int y);
TileType GetTileType(const Tilemap *tilemap, int layerIndex, int x, int y);

#endif