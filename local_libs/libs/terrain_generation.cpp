#include "terrain_generation.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <random>


void TerrainGenerator::GenerateTerrain(const int width, const int height, const int channels) {

    // Inizializza il generatore di numeri casuali
    std::random_device rd;
    std::mt19937 gen(rd()); // Utilizza il Mersenne Twister come generatore

    // Genera una distribuzione uniforme di numeri interi tra 0 e 255
    std::uniform_int_distribution<> distribution(0, 255);
    unsigned char* image = new unsigned char[width * height * channels]; // array per i dati dell'immagine

    // Popoliamo l'array con un'immagine di esempio (in questo caso, un gradiente)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image[(y * width + x) * channels + 0] = static_cast<unsigned char>(distribution(gen)); // Red
        }
    }

    // Applica una sfumatura gaussiana all'immagine
    const int radius = 2; // Raggio della sfumatura
    unsigned char* blurredImage = new unsigned char[width * height * channels]; // Array per l'immagine sfocata

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Calcola il valore medio dei pixel circostanti per ciascun canale
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;
                int count = 0;
                for (int oy = -radius; oy <= radius; ++oy) {
                    for (int ox = -radius; ox <= radius; ++ox) {
                        int sx = x + ox;
                        int sy = y + oy;
                        if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                            sum += static_cast<float>(image[(sy * width + sx) * channels + c]);
                            ++count;
                        }
                    }
                }
                blurredImage[(y * width + x) * channels + c] = static_cast<unsigned char>(sum / count);
            }
        }
    }

    

    // Salva l'immagine sfumata come file PNG
    stbi_write_png("resources/textures/blurred_terrain.png", width, height, channels, blurredImage, width * channels);

    delete[] image; // Libera la memoria dell'immagine originale
    delete[] blurredImage; // Libera la memoria dell'immagine sfocata
}
