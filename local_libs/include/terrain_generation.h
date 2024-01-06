#ifndef TERRAIN_GENERATION_H
#define TERRAIN_GENERATION_H
#include <vector>

class TerrainGenerator {
public:
    static void GenerateTerrain(const int width, const int height, const int channels);
};

#endif