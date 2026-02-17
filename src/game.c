
#include "game.h"

#include <stdio.h>
#include <stdlib.h>

Item LoadItem(const char *filename, const EquipSlot slot, const char *label) {
    Item item = {0};
    item.texture = LoadTextureFromBin(filename);
    item.slot = slot;
    item.label = label;

    return item;
}

void UnloadItem(const Item item) {
    UnloadTexture(item.texture);
}

Texture2D LoadTextureFromBin(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        TraceLog(LOG_ERROR, "Gagal buka file binary: %s", fileName);
        return (Texture2D){ 0 };
    }

    unsigned int size = 0;
    // Baca 4 byte pertama (size)
    if (fread(&size, sizeof(unsigned int), 1, file) != 1) {
        fclose(file);
        return (Texture2D){ 0 };
    }

    // Alokasi memori sesuai size
    unsigned char *data = (unsigned char *)malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    // Raylib proses datanya
    Image img = LoadImageFromMemory(".png", data, size);
    Texture2D tex = LoadTextureFromImage(img);

    UnloadImage(img);
    free(data);

    return tex;
}