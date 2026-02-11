
#include "game.h"

Item LoadItem(const char *filename, EquipSlot slot, const char *label) {
    Item item = {0};
    item.texture = LoadTexture(filename);
    item.slot = slot;
    item.label = label;

    return item;
}

void UnloadItem(Item item) {
    UnloadTexture(item.texture);
}
