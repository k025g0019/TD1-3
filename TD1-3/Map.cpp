#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.h"

int LoadMapCSV(const char* filePath, int map[MAP_HEIGHT][MAP_WIDTH])
{
    FILE* fp = NULL;

    if (fopen_s(&fp, filePath, "r") != 0 || fp == NULL) {
        return 0;
    }

    char line[LINE_BUF_SIZE];
    int y = 0;

    while (fgets(line, (int)sizeof(line), fp) != NULL && y < MAP_HEIGHT) {
        line[strcspn(line, "\r\n")] = '\0';

        int x = 0;
        char* context = NULL;
        char* token = strtok_s(line, ",", &context);

        while (token != NULL && x < MAP_WIDTH) {
            map[y][x] = atoi(token);
            token = strtok_s(NULL, ",", &context);
            x++;
        }


        while (x < MAP_WIDTH) {
            map[y][x] = -1;
            x++;
        }

        y++;
    }


    while (y < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = -1;
        }
        y++;
    }

    fclose(fp);
    return 1;
}
