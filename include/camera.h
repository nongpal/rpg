#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "player.h"
#include "tilemap.h"

typedef struct {
    Camera2D camera;
    float followSpeed;
    float zoom;
    Vector2 offset;
    bool smoothFollow;
    Vector2 boundsMin;
    Vector2 boundsMax;
} GameCamera;

GameCamera InitGameCamera(int screenWidth, int screenHeight);
void UpdateCamera2D(GameCamera* camera, const Player* player, const Tilemap* map);
void SetCameraBounds(GameCamera* camera, const Tilemap* map);
void SetCameraZoom(GameCamera* camera, float zoom);

#endif