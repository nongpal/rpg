#include "collusion.h"

#include <assert.h>
#include <stdlib.h>

Collusion CreateCollusion(const Tilemap *tilemap)
{
    assert(tilemap != NULL && "[ERROR] Tilemap not found!\n");
    const TileLayer *layer = &tilemap->layers[LAYER_COLLUSION];

    bool *visited = calloc(layer->width * layer->height, sizeof(bool));
    unsigned int totalRect = 0;

    for (int y = 0; y < layer->height; y++) {
        for (int x = 0; x < layer->width; x++) {
            if (layer->data[y * layer->width + x] > 0 && !visited[y * layer->width + x]) {
                totalRect++;

                int currentW = 1;
                while (x + 1 < layer->width && layer->data[y * layer->width + (x + currentW)] > 0 &&
                        !visited[y * layer->width + (x + currentW)]) {

                    currentW++;
                }

                int currentH = 1;
                while (y + currentH < layer->height) {
                    bool canExpand = true;
                    for (int i = 0; i < currentW; i++) {
                        if (layer->data[(y + currentH) * layer->width + (x + i)] == 0 ||
                            visited[(y + currentH) * layer->width + (x + i)]) {

                            canExpand = false;
                            break;
                        }
                    }

                    if (canExpand) currentW++;
                    else break;
                }

                for (int row = 0; row < currentH; row++) {
                    for (int col = 0; col < currentW; col++) {
                        visited[(y + row) * layer->width + (x + col)] = true;
                    }
                }
            }
        }
    }

    free(visited);

    Collusion collusion = {0};
    collusion.rectCount = totalRect;
    collusion.rect = malloc(totalRect * sizeof(Rectangle));
    assert(collusion.rect != NULL && "[ERROR] Failed alloc collusion!\n");

    return collusion;
}

void SetupCollusion(Collusion *collusion, const Tilemap *tilemap)
{
    if (!collusion || !tilemap) return;
    const TileLayer *layer = &tilemap->layers[LAYER_COLLUSION];

    bool *visited = calloc(layer->width * layer->height, sizeof(bool));
    int rectIdx = 0;

    for (int y = 0; y < layer->height; y++) {
        for (int x = 0; x < layer->width; x++) {

            if (layer->data[y * layer->width + x] > 0 && !visited[y * layer->width + x]) {

                int currentW = 1;
                while (x + currentW < layer->width && layer->data[y * layer->width + (x + currentW)] > 0 &&
                        !visited[y * layer->width + (x + currentW)]) {
                    currentW++;
                }

                int currentH = 1;
                while (y + currentH < layer->height) {
                    bool canExpand = true;
                    for (int i = 0; i < currentW; i++) {
                        if (layer->data[(y + currentH) * layer->width + (x + i)] == 0 ||
                            visited[(y + currentH) * layer->width + (x + i)]) {
                            canExpand = false;
                            break;
                        }
                    }

                    if (canExpand) currentH++;
                    else break;
                }

                for (int row = 0; row < currentH; row++) {
                    for (int col = 0; col < currentW; col++) {
                        visited[(y + row) * layer->width + (x + col)] = true;
                    }
                }

                collusion->rect[rectIdx].x = (float)x* TILE_SIZE;
                collusion->rect[rectIdx].y = (float)y * TILE_SIZE;
                collusion->rect[rectIdx].width = (float)currentW * TILE_SIZE;
                collusion->rect[rectIdx].height = (float)currentH * TILE_SIZE;

                rectIdx++;
            }
        }
    }

    free(visited);
}

void DestroyCollusion(Collusion *collusion)
{
    if (collusion->rect) free(collusion->rect);
    collusion->rect = 0;
    collusion->width = 0;
    collusion->height = 0;
}