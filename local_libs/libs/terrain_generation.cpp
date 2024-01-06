#include "terrain_generation.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void TerrainGenerator::GenerateTerrain(const int width, const int height, const int channels) {
    
    unsigned char* image = new unsigned char[width * height * channels]; // array per i dati dell'immagine

    // Popoliamo l'array con un'immagine di esempio (in questo caso, un gradiente)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Calcoliamo un gradiente rosso e verde sull'immagine
            image[(y * width + x) * channels + 0] = static_cast<unsigned char>((x / (float)width) * 255); // Red
            image[(y * width + x) * channels + 1] = static_cast<unsigned char>((x / (float)width) * 255);//static_cast<unsigned char>((y / (float)height) * 255); // Green
            image[(y * width + x) * channels + 2] = static_cast<unsigned char>((x / (float)width) * 255); // Blue (zero)
            image[(y * width + x) * channels + 3] = 255; // Alpha (non trasparente)
        }
    }

    // Salva l'immagine come file PNG
    stbi_write_png("resources/textures/generated_terrain.png", width, height, channels, image, width * channels);

    //What is not working? 


    delete[] image; // Libera la memoria
}
