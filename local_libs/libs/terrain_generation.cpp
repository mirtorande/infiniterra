#include "terrain_generation.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "FastNoiseLite.h"
#include <random>


void TerrainGenerator::GenerateTerrain(const int width, const int height, const int channels) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(2.0f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFractalLacunarity(2.0f);
    // Il fractal gain è il fattore di attenuazione del rumore, minore è il valore, più liscio sarà il terreno
    noise.SetFractalGain(0.4f);
    // To make it so that the noise starts from specific coordinates, we can set the seed
    noise.SetSeed(12345);
 

    unsigned char* image = new unsigned char[width * height * channels]; // Array per i dati dell'immagine

    // Popoliamo l'array con Ridged Perlin Noise
    float minNoise = 9999999.0f;
    float maxNoise = -9999999.0f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float nx = static_cast<float>(x) / static_cast<float>(width);
            float ny = static_cast<float>(y) / static_cast<float>(height);

            float value = noise.GetNoise(nx, ny);
            // Applica l'operazione ABS() per ottenere il rumore ridged
            value = fabsf(value);

            // Inverti i valori per far sì che la cresta si verifichi ai valori più alti
            value = 1.0f - value;

            // Converte il valore in unsigned char (0-255)
            image[(y * width + x) * channels] = static_cast<unsigned char>((value * 255.0f) + 0.5f);

            // Trova i valori minimo e massimo del rumore per normalizzare
            if (value < minNoise) minNoise = value;
            if (value > maxNoise) maxNoise = value;
        }
    }

    // Normalizza i valori del rumore
    for (int i = 0; i < width * height * channels; ++i) {
        image[i] = static_cast<unsigned char>((image[i] - minNoise * 255.0f) / (maxNoise - minNoise) + 0.5f);
    }

    // Salva l'immagine come file PNG
    stbi_write_png("resources/textures/generated_terrain.png", width, height, channels, image, width * channels);

    delete[] image; // Libera la memoria dell'immagine originale
}

