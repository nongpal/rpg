#ifndef TILEMAP_H
#define TILEMAP_H

#include "raylib.h"

#define TILE_SIZE   32
#define SUB_TILE    16

typedef enum {
    LAYER_GROUND,
    LAYER_COLLUSION,
} LayerType;

typedef enum {
    TOP_LEFT = 0,
    TOP_RIGHT,
    BOTTOM_RIGHT,
    BOTTOM_LEFT
} CornerSide;

typedef enum {
    TOP = 0,
    RIGHT,
    BOTTOM,
    LEFT
} EdgeSide;

typedef struct {
    int x, y;
    Texture2D texture;
    Rectangle corner_left[4];
    Rectangle corner_right[4];  // 16x16 {top-left, top-right, bottom-right, bottom-left}
    Rectangle edge[4];          // 16x32 / 32/16 {top, right, bottom, left}
} AutoTile;

typedef struct {
    Texture2D texture;
    int firstGid;
    char *source;
} Tileset;

typedef struct {
    unsigned int id;
    char *name;
    unsigned int *data;
    int x, y;
    unsigned int width, height;
    bool visible;
} TileLayer;

typedef struct {
    int width, height;
    int tileWidth, tileHeight;

    Tileset *tilesets;
    int tilesetCount;

    TileLayer *layers;
    int layerCount;

    AutoTile *autoTile;
} Tilemap;


Tilemap LoadTilemap(const char *json_file);
void UnloadTilemap(Tilemap map);
bool LoadAutoTile(Tilemap *map, const char *texture_file, float x, float y);
RenderTexture2D SetupTextureMode(Tilemap *map);
void DrawAutoTile(Tilemap *map);


#endif