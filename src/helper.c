#include "helper.h"

#include <stdio.h>
#include <stdlib.h>

char *ReadFile2String(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "[ERROR] Cannot open file: %s\n", filename);
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "[ERROR] Cannot seek to end of file: %s\n", filename);
        fclose(file);
        return NULL;
    }

    const long length = ftell(file);
    if (length < 0) {
        fprintf(stderr, "[ERROR] Cannot get file size: %s\n", filename);
        fclose(file);
        return NULL;
    }

    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "[ERROR] Memory allocation failed for file: %s\n", filename);
        fclose(file);
        return NULL;
    }

    const size_t read = fread(buffer, 1, length, file);
    if (read != (size_t)length) {
        fprintf(stderr, "[ERROR] Incomplete read from file: %s\n", filename);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[read] = '\0';
    fclose(file);
    return buffer;
}
