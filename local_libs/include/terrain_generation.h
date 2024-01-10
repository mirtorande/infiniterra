#ifndef TERRAIN_GENERATION_H
#define TERRAIN_GENERATION_H
#include <vector>

class TerrainGenerator {
public:
    static unsigned char* GenerateTerrainHeights(const int width, const int height, const int widthOffset, const int heightOffset, const int channels);

    static std::vector<float> GenerateChunkGeometry(const int width, const int height, const unsigned rez);
};

#endif