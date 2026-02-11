#ifndef GAME_H
#define GAME_H

#define MAX_STRING      1024
#define SHORT_STRING    256

#include "raylib.h"

typedef enum {
    DIRECTION_FRONT,
    DIRECTION_BACK,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;

typedef enum {
    SLOT_TOP,
    SLOT_MASK,
    SLOT_HAIR,
    SLOT_BOTTOM,
    SLOT_FEET,
    SLOT_HAND_MAIN,
    SLOT_HAND_OFF,
    SLOT_ACCESSORIES,
    MAX_SLOTS
} EquipSlot;

typedef enum {
    ACTION_IDLE,
    ACTION_WALK,
    ACTION_RUN,
    ACTION_SNEAK
} Action;

typedef struct {
    Texture2D texture;
    EquipSlot slot;
    const char *label;
} Item;

Item LoadItem(const char *filename, EquipSlot slot, const char *label);
void UnloadItem(Item item);

#endif
