#ifndef IVY_COLLUSION_H
#define IVY_COLLUSION_H

#include "raylib.h"
#include "tilemap.h"

typedef struct {
    Rectangle *rect;
    unsigned int rectCount;
    unsigned int width, height;
} Collusion;

Collusion CreateCollusion(const Tilemap *tilemap);
void SetupCollusion(Collusion *collusion, const Tilemap *tilemap);
void DestroyCollusion(Collusion *collusion);

#endif