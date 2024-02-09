#ifndef TERRAIN_GENERATION_H
#define TERRAIN_GENERATION_H
#include <vector>

class TerrainGenerator {
public:
    static std::vector<float> GenerateChunkGeometry(const int width, const int height, const unsigned rez);
};

#endif