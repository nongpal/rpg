#include "camera.h"
#include "raymath.h"

GameCamera InitGameCamera(const int screenWidth, const int screenHeight)
{
    GameCamera camera = {0};
    
    camera.camera.target    = (Vector2){ 0.0f, 0.0f };
    camera.camera.offset    = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    camera.camera.rotation  = 0.0f;
    camera.camera.zoom      = 1.0f;
    
    camera.followSpeed      = 5.0f;
    camera.zoom             = 2.0f;
    camera.zoomTarget       = 2.0f;
    camera.offset           = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    camera.smoothFollow     = true;
    camera.boundsMin        = (Vector2){ 0.0f, 0.0f };
    camera.boundsMax        = (Vector2){ 1000.0f, 1000.0f };
    
    return camera;
}

void UpdateCamera2D(GameCamera *camera, const Player *player, const Tilemap *map, const float frameTime)
{
    if (!camera || !player || !map) return;

    const float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera->zoomTarget += wheel * 0.25f;
        camera->zoomTarget = Clamp(camera->zoomTarget, 0.5f, 3.0f);
    }

    const float zoomLerpFactor = 1.0f - expf(-8.0f * frameTime);
    camera->zoom = Lerp(camera->zoom, camera->zoomTarget, zoomLerpFactor);

    camera->camera.zoom = camera->zoom;

    const Vector2 targetPos = {
        player->movement.position.x + 16,
        player->movement.position.y + 16
    };

    if (camera->smoothFollow) {
        const float dt = GetFrameTime();
        const float lerpFactor = 1.0f - expf(-camera->followSpeed * dt);
        camera->camera.target = Vector2Lerp(camera->camera.target, targetPos, lerpFactor);
    } else {
        camera->camera.target = targetPos;
    }

    const Vector2 viewHalfSize = {
        (float)GetScreenWidth() / camera->camera.zoom * 0.5f,
        (float)GetScreenHeight() / camera->camera.zoom * 0.5f
    };

    const float mapW = (float)(map->width * map->tileWidth);
    const float mapH = (float)(map->height * map->tileHeight);

    camera->camera.target.x = Clamp(camera->camera.target.x, viewHalfSize.x, mapW - viewHalfSize.x);
    camera->camera.target.y = Clamp(camera->camera.target.y, viewHalfSize.y, mapH - viewHalfSize.y);
}

void SetCameraBounds(GameCamera* camera, const Tilemap* map) {
    if (!camera || !map) return;
    
    camera->boundsMin = (Vector2){ 0.0f, 0.0f };
    camera->boundsMax = (Vector2){ 
        (float)(map->width * map->tileWidth),
        (float)(map->height * map->tileHeight)
    };
}

void SetCameraZoom(GameCamera* camera, float zoom) {
    if (!camera) return;
    
    if (zoom < 0.5f) zoom = 0.5f;
    if (zoom > 3.0f) zoom = 3.0f;
    
    camera->zoom = zoom;
}