#include "terrain_generation.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "FastNoiseLite.h"
#include <random>


void TerrainGenerator::GenerateTerrain(const int width, const int height, const int channels) {

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);


    unsigned char* image = new unsigned char[width * height * channels]; // array per i dati dell'immagine

    // Popoliamo l'array
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image[(y * width + x) * channels] = static_cast<unsigned char>(noise.GetNoise((float)x, (float)y)*5.0f); // Red
        }
    }


    // Salva l'immagine come file PNG
    stbi_write_png("resources/textures/generated_terrain.png", width, height, channels, image, width * channels);

    delete[] image; // Libera la memoria dell'immagine originale
}
