#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tilemap.h"
#include "cJSON.h"
#include "game.h"
#include "raylib.h"
#include "helper.h"


#define MAX_PATH_LENGTH 256
#define MAX_NAME_LENGTH 64
#define TILESET_PATH_PREFIX "../assets/tilesets/"


static bool LoadLayers(Tilemap* tilemap, const cJSON* layers_node);
static bool LoadTilesets(Tilemap *map, const cJSON *tilesets);
static bool IsTilePresent(const TileLayer* layer, int x, int y, int W, int H);
static void RenderBaseTiles(const Tilemap* map);
static void RenderAutotileEdges(const Tilemap* map, const TileLayer* layer, int W, int H, float TW, float TH);
static void RenderAutotileCorners(const Tilemap* map, const TileLayer* layer, int W, int H, float TW, float TH);


static bool
LoadLayers(Tilemap* tilemap, const cJSON* layers_node)
{
    const int total = cJSON_GetArraySize(layers_node);
    if (total <= 0) {
        fprintf(stderr, "[WARNING] No layers found\n");
        return false;
    }

    tilemap->layers = calloc(total, sizeof(TileLayer));
    if (!tilemap->layers) {
        fprintf(stderr, "[ERROR] Memory allocation failed for layers\n");
        return false;
    }

    tilemap->layerCount = 0;

    const cJSON* item = NULL;
    cJSON_ArrayForEach(item, layers_node)
    {
        if (tilemap->layerCount >= total) break;

        TileLayer* layer = &tilemap->layers[tilemap->layerCount];

        const cJSON* data_arr   = cJSON_GetObjectItem(item, "data");
        const cJSON* name       = cJSON_GetObjectItem(item, "name");
        const cJSON* id_obj     = cJSON_GetObjectItem(item, "id");
        const cJSON* width_obj  = cJSON_GetObjectItem(item, "width");
        const cJSON* height_obj = cJSON_GetObjectItem(item, "height");

        if (!cJSON_IsNumber(id_obj) || !cJSON_IsNumber(width_obj) ||
            !cJSON_IsNumber(height_obj) || !cJSON_IsArray(data_arr)) {
            fprintf(stderr, "[WARNING] Invalid layer entry\n");
            continue;
        }

        layer->id       = id_obj->valueint;
        layer->width    = width_obj->valueint;
        layer->height   = height_obj->valueint;
        layer->visible  = cJSON_IsTrue(cJSON_GetObjectItem(item, "visible"));
        layer->name     = name && name->valuestring ? strdup(name->valuestring) : NULL;

        const int dataSize = cJSON_GetArraySize(data_arr);
        if (dataSize > 0) {
            layer->data = malloc(sizeof(unsigned int) * dataSize);
            if (!layer->data) {
                fprintf(stderr, "[ERROR] Memory allocation failed for layer data\n");
                if (layer->name) free(layer->name);
                continue;
            }

            int i = 0;
            const cJSON* val = NULL;
            cJSON_ArrayForEach(val, data_arr) {
                if (i >= dataSize) break;
                layer->data[i++] = (unsigned int)val->valueint;
            }
        }

        tilemap->layerCount++;
    }

    return tilemap->layerCount > 0;
}

static bool
LoadTilesets(Tilemap *map, const cJSON *tilesets)
{
    if (!tilesets) return false;

    const int total = cJSON_GetArraySize(tilesets);
    if (total <= 0) return false;

    map->tilesets = calloc(total, sizeof(Tileset));
    if (!map->tilesets) return false;

    map->tilesetCount = 0;
    const cJSON *item = NULL;

    cJSON_ArrayForEach(item, tilesets)
    {
        const cJSON *firstGid = cJSON_GetObjectItem(item, "firstgid");
        const cJSON *source   = cJSON_GetObjectItem(item, "source");

        if (!cJSON_IsNumber(firstGid) || !cJSON_IsString(source)) continue;

        const char *fullPath = source->valuestring;
        const char *fileName = strrchr(fullPath, '/');
        fileName = !fileName ? fullPath : fileName + 1;

        char baseName[SHORT_STRING] = {0};
        size_t len = strcspn(fileName, ".");
        if (len >= SHORT_STRING) len = SHORT_STRING - 1;
        strncpy(baseName, fileName, len);

        char finalPath[MAX_STRING];
        snprintf(finalPath, sizeof(finalPath), "%s%s.png", TILESET_PATH_PREFIX, baseName);

        Tileset *ts = &map->tilesets[map->tilesetCount];
        const Texture2D tex = LoadTexture(finalPath);

        if (tex.id > 0)
        {
            ts->firstGid = firstGid->valueint;
            ts->source   = strdup(finalPath);
            ts->texture  = tex;
            map->tilesetCount++;
        } else {
            fprintf(stderr, "[ERROR] Failed to load tileset texture: %s\n", finalPath);
        }
    }

    return map->tilesetCount > 0;
}

static bool
IsTilePresent(const TileLayer* layer, const int x, const int y, const int W, const int H)
{
    return x >= 0 && x < W && y >= 0 && y < H && layer->data[y * W + x] != 0;
}

static void
RenderBaseTiles(const Tilemap* map)
{
    const TileLayer* layer  = &map->layers[0];
    const float TW          = (float)map->tileWidth;
    const float TH          = (float)map->tileHeight;
    const int W             = (int)layer->width;
    const int H             = (int)layer->height;

    const Texture2D atlas   = map->tilesets[0].texture;
    const int atlasCol      = atlas.width / (int)TW;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const unsigned int gid = layer->data[y * W + x];
            if (gid == 0) continue;

            const int localId       = (int)gid - map->tilesets[0].firstGid;
            const Rectangle gSrc    = { (float)(localId % atlasCol) * TW, (float)(localId / atlasCol) * TH, TW, TH};
            const Vector2 gPos      = { (float)x * TW, (float)y * TH };

            DrawTextureRec(atlas, gSrc, gPos, WHITE);
        }
    }
}

static void
RenderAutotileEdges(const Tilemap* map, const TileLayer* layer, const int W, const int H, const float TW, const float TH) {
    const float hW = TW / 2.0f;
    const float hH = TH / 2.0f;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            unsigned int gid = layer->data[y * W + x];
            if (gid == 0) continue;

            const Vector2 gPos = { (float)x * TW, (float)y * TH };

            const bool N_empty = !IsTilePresent(layer, x, y - 1, W, H);
            const bool S_empty = !IsTilePresent(layer, x, y + 1, W, H);
            const bool E_empty = !IsTilePresent(layer, x + 1, y, W, H);
            const bool W_empty = !IsTilePresent(layer, x - 1, y, W, H);

            if (W_empty) DrawTextureRec(map->autoTile->texture, map->autoTile->edge[LEFT],      (Vector2){ gPos.x - hW, gPos.y }, WHITE);
            if (E_empty) DrawTextureRec(map->autoTile->texture, map->autoTile->edge[RIGHT],     (Vector2){ gPos.x + TW, gPos.y }, WHITE);
            if (N_empty) DrawTextureRec(map->autoTile->texture, map->autoTile->edge[TOP],       (Vector2){ gPos.x, gPos.y - hH }, WHITE);
            if (S_empty) DrawTextureRec(map->autoTile->texture, map->autoTile->edge[BOTTOM],    (Vector2){ gPos.x, gPos.y + TH }, WHITE);
        }
    }
}

static void
RenderAutotileCorners(const Tilemap* map, const TileLayer* layer, const int W, const int H, const float TW, const float TH) {
    const float hW = TW / 2.0f;
    const float hH = TH / 2.0f;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const unsigned int gid = layer->data[y * W + x];
            if (gid == 0) continue;

            const Vector2 gPos = { (float)x * TW, (float)y * TH };

            const bool N_empty  = !IsTilePresent(layer, x, y - 1, W, H);
            const bool S_empty  = !IsTilePresent(layer, x, y + 1, W, H);
            const bool E_empty  = !IsTilePresent(layer, x + 1, y, W, H);
            const bool W_empty  = !IsTilePresent(layer, x - 1, y, W, H);
            const bool NW_empty = !IsTilePresent(layer, x - 1, y - 1, W, H);
            const bool NE_empty = !IsTilePresent(layer, x + 1, y - 1, W, H);
            const bool SW_empty = !IsTilePresent(layer, x - 1, y + 1, W, H);
            const bool SE_empty = !IsTilePresent(layer, x + 1, y + 1, W, H);

            if (W_empty && N_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_right[TOP_LEFT],
                              (Vector2){ gPos.x - hW, gPos.y - hH }, WHITE);
            if (E_empty && N_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_right[TOP_RIGHT],
                              (Vector2){ gPos.x + TW, gPos.y - hH }, WHITE);
            if (W_empty && S_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_right[BOTTOM_LEFT],
                              (Vector2){ gPos.x - hW, gPos.y + TH }, WHITE);
            if (E_empty && S_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_right[BOTTOM_RIGHT],
                              (Vector2){ gPos.x + TW, gPos.y + TH }, WHITE);

            if (!N_empty && !W_empty && NW_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_left[BOTTOM_RIGHT],
                              (Vector2){ gPos.x - hW, gPos.y - hH }, WHITE);
            if (!N_empty && !E_empty && NE_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_left[BOTTOM_LEFT],
                              (Vector2){ gPos.x + TW, gPos.y - hH }, WHITE);
            if (!S_empty && !W_empty && SW_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_left[TOP_RIGHT],
                              (Vector2){ gPos.x - hW, gPos.y + TH }, WHITE);
            if (!S_empty && !E_empty && SE_empty)
                DrawTextureRec(map->autoTile->texture, map->autoTile->corner_left[TOP_LEFT],
                              (Vector2){ gPos.x + TW, gPos.y + TH }, WHITE);
        }
    }
}

Tilemap LoadTilemap(const char* json_file) {
    Tilemap tilemap = {0};

    char* json_string = ReadFile2String(json_file);
    if (!json_string) return tilemap;

    cJSON* root = cJSON_Parse(json_string);
    if (!root)
    {
        fprintf(stderr, "[ERROR] JSON Parse Error\n");
        free(json_string);
        return tilemap;
    }

    tilemap.width       = cJSON_GetObjectItem(root, "width")->valueint;
    tilemap.height      = cJSON_GetObjectItem(root, "height")->valueint;
    tilemap.tileWidth   = cJSON_GetObjectItem(root, "tilewidth")->valueint;
    tilemap.tileHeight  = cJSON_GetObjectItem(root, "tileheight")->valueint;

    const cJSON* tilesets_node  = cJSON_GetObjectItem(root, "tilesets");
    const cJSON* layers_node    = cJSON_GetObjectItem(root, "layers");

    const bool tilesets_ok  = LoadTilesets(&tilemap, tilesets_node);
    const bool layers_ok    = LoadLayers(&tilemap, layers_node);

    if (!tilesets_ok || !layers_ok) UnloadTilemap(tilemap);

    cJSON_Delete(root);
    free(json_string);

    return tilemap;
}

void UnloadTilemap(const Tilemap map)
{
    for (int i = 0; i < map.tilesetCount; i++) {
        if (map.tilesets[i].source) free(map.tilesets[i].source);
        UnloadTexture(map.tilesets[i].texture);
    }
    free(map.tilesets);

    for (int i = 0; i < map.layerCount; i++) {
        free(map.layers[i].data);
        if (map.layers[i].name) free(map.layers[i].name);
    }
    free(map.layers);

    if (map.autoTile) {
        UnloadTexture(map.autoTile->texture);
        free(map.autoTile);
    }
}

bool LoadAutoTile(Tilemap* map, const char* texture_file, const float x, const float y)
{
    if (!map) {
        fprintf(stderr, "[ERROR] NULL tilemap provided\n");
        return false;
    }

    if (map->autoTile) {
        UnloadTexture(map->autoTile->texture);
        free(map->autoTile);
        map->autoTile = NULL;
    }

    map->autoTile = malloc(sizeof(AutoTile));
    if (!map->autoTile) {
        fprintf(stderr, "[ERROR] Memory allocation failed for autotile\n");
        return false;
    }

    map->autoTile->texture = LoadTexture(texture_file);
    if (map->autoTile->texture.id <= 0) {
        fprintf(stderr, "[ERROR] Failed to load autotile texture: %s\n", texture_file);
        free(map->autoTile);
        map->autoTile = NULL;
        return false;
    }

    map->autoTile->x = (int)x;
    map->autoTile->y = (int)y;

    map->autoTile->corner_left[TOP_LEFT]        = (Rectangle){ x, y, SUB_TILE, SUB_TILE };
    map->autoTile->corner_left[TOP_RIGHT]       = (Rectangle){ x + SUB_TILE, y, SUB_TILE, SUB_TILE };
    map->autoTile->corner_left[BOTTOM_RIGHT]    = (Rectangle){ x + SUB_TILE, y + SUB_TILE, SUB_TILE, SUB_TILE };
    map->autoTile->corner_left[BOTTOM_LEFT]     = (Rectangle){ x, y + SUB_TILE, SUB_TILE, SUB_TILE };

    map->autoTile->corner_right[BOTTOM_RIGHT]   = (Rectangle){ x + TILE_SIZE, y, SUB_TILE, SUB_TILE };
    map->autoTile->corner_right[BOTTOM_LEFT]    = (Rectangle){ x + TILE_SIZE + SUB_TILE, y, SUB_TILE, SUB_TILE };
    map->autoTile->corner_right[TOP_LEFT]       = (Rectangle){ x + TILE_SIZE + SUB_TILE, y + SUB_TILE, SUB_TILE, SUB_TILE };
    map->autoTile->corner_right[TOP_RIGHT]      = (Rectangle){ x + TILE_SIZE, y + SUB_TILE, SUB_TILE, SUB_TILE };

    map->autoTile->edge[BOTTOM] = (Rectangle){ x + SUB_TILE, y + TILE_SIZE, TILE_SIZE, SUB_TILE };
    map->autoTile->edge[TOP]    = (Rectangle){ x + SUB_TILE, y + TILE_SIZE * 2.0f + SUB_TILE, TILE_SIZE, SUB_TILE };
    map->autoTile->edge[LEFT]   = (Rectangle){ x + TILE_SIZE + SUB_TILE, y + TILE_SIZE + SUB_TILE, SUB_TILE, TILE_SIZE };
    map->autoTile->edge[RIGHT]  = (Rectangle){ x, y + TILE_SIZE + SUB_TILE, SUB_TILE, TILE_SIZE };

    return true;
}

RenderTexture2D SetupTextureMode(Tilemap* map)
{
    if (!map || map->layerCount <= 0 || map->tilesetCount <= 0)
    {
        fprintf(stderr, "[ERROR] Invalid tilemap for rendering\n");
        return (RenderTexture2D){0};
    }

    const RenderTexture2D canvas = LoadRenderTexture(map->width * map->tileWidth, map->height * map->tileHeight);

    BeginTextureMode(canvas);
    ClearBackground(BLANK);

    RenderBaseTiles(map);

    if (map->autoTile) {
        const TileLayer* layer = &map->layers[0];
        const int W     = (int)layer->width;
        const int H     = (int)layer->height;
        const float TW  = (float)map->tileWidth;
        const float TH  = (float)map->tileHeight;

        RenderAutotileEdges(map, layer, W, H, TW, TH);
        RenderAutotileCorners(map, layer, W, H, TW, TH);
    }

    EndTextureMode();
    return canvas;
}
