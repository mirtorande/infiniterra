#include "terrain_generation.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"

#include "stb_image_write.h"
#include "FastNoiseLite.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

unsigned char* TerrainGenerator::GenerateTerrainHeights(const int width, const int height, const int widthOffset, const int heightOffset, const int channels)
{
    FastNoiseLite mountainRangeNoise;
    mountainRangeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mountainRangeNoise.SetFrequency(4.0f);
    //mountainRangeNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    //mountainRangeNoise.SetFractalOctaves(4);
    //mountainRangeNoise.SetFractalLacunarity(2.0f);
    // Il fractal gain è il fattore di attenuazione del rumore, minore è il valore, più liscio sarà il terreno
    //mountainRangeNoise.SetFractalGain(0.4f);
    // To make it so that the noise starts from specific coordinates, we can set the seed
    mountainRangeNoise.SetSeed(12345);

    FastNoiseLite biomeNoise;
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Value);
    biomeNoise.SetFrequency(1.0f);
    //biomeNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    biomeNoise.SetSeed(12345);
 

    unsigned char* image = new unsigned char[width * height * channels]; // Array per i dati dell'immagine

    // Popoliamo l'array con Ridged Perlin Noise

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float nx = static_cast<float>(x + widthOffset) / static_cast<float>(width);
            float ny = static_cast<float>(y + heightOffset) / static_cast<float>(height);

            float value = mountainRangeNoise.GetNoise(nx, ny);
            // Applica l'operazione ABS() per ottenere il rumore ridged
            value = fabsf(value);


            // Inverti i valori per far sì che la cresta si verifichi ai valori più alti. Moltiplico per 1.5 per aumentare il range di valori raggunti (Valore empirico)
            value = 1.0f - 1.5f * value;

            // Moltiplica per il valore del bioma, risacalato tra 0 e 1
            value = value * 0.5f * (biomeNoise.GetNoise(nx, ny) + 1.0f);

            // Converte il valore in unsigned char (0-255)
            image[(y * width + x) * channels] = static_cast<unsigned char>(value * 255.0f);

        }
    }

    // Salva l'immagine come file PNG
    //stbi_write_png("resources/textures/generated_terrain.png", width, height, channels, image, width * channels);

    return image;
}

std::vector<float> TerrainGenerator::GenerateChunkGeometry(const int width, const int height, const unsigned rez)
{
    std::vector<float> vertices;
    for (unsigned i = 0; i <= rez - 1; i++)
    {
        for (unsigned j = 0; j <= rez - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
        }
    }
    //std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    //std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;
    return vertices;
}
