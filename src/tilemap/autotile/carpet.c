#include "ivy/tilemap/tilemap.h"

void TM_DrawTileCarpet(const Tilemap *tilemap, const Tileset *tileset,
                    const Rectangle src, const Vector2 pos,
                    const u32 x, const u32 y)
{
    const TileType typeN = TM_GetTileType(tilemap, 1, x, y - 1);
    const TileType typeS = TM_GetTileType(tilemap, 1, x, y + 1);
    const TileType typeE = TM_GetTileType(tilemap, 1, x + 1, y);
    const TileType typeW = TM_GetTileType(tilemap, 1, x - 1, y);

    const float sX = src.x;
    const float sY = src.y;
    const float pX = pos.x;
    const float pY = pos.y;

    const float tileSize = (float)tilemap->header.tileWidth;
    const float tileHalf = tileSize * 0.5f;
    const Texture2D tex = tileset->texture;

    if (typeE == TILE_CARPET) {
        DrawTextureRec(tex, (Rectangle){ sX + tileHalf, sY + tileSize + tileHalf, tileSize, tileSize }, (Vector2){ pX, pY }, WHITE);
    }

    if (typeE != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX + tileSize, sY + tileSize + tileHalf, tileSize, tileSize }, (Vector2){ pX, pY }, WHITE);

    if (typeW != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX, sY + tileSize + tileHalf, tileSize, tileSize }, (Vector2){ pX, pY }, WHITE);

    if (typeN != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX + tileHalf, sY + tileSize, tileSize, tileHalf }, (Vector2){ pX, pY }, WHITE);

    if (typeS != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX + tileHalf, sY + tileSize * 2 + tileHalf, tileSize, tileHalf }, (Vector2){ pX + 0, pY + tileHalf }, WHITE);

    if (typeN != TILE_CARPET && typeE != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX + tileSize + tileHalf, sY + tileSize, tileHalf, tileHalf }, (Vector2){ pX + tileHalf, pY }, WHITE);

    if (typeN != TILE_CARPET && typeW != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX, sY + tileSize, tileHalf, tileHalf }, (Vector2){ pX, pY }, WHITE);

    if (typeS != TILE_CARPET && typeE != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX + tileSize + tileHalf, sY + 2 * tileSize + tileHalf, tileHalf, tileHalf }, (Vector2){ pX + tileHalf, pY + tileHalf }, WHITE);

    if (typeS != TILE_CARPET && typeW != TILE_CARPET)
        DrawTextureRec(tex, (Rectangle){ sX, sY + 2 * tileSize + tileHalf, tileHalf, tileHalf }, (Vector2){ pX, pY + tileHalf }, WHITE);
}