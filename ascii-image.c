#include <stdio.h>
#include <string.h>  
#include <stdlib.h>  
#include <stdint.h>  

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"


const char *ASCII_CHARS = " .'`^,:;Il!i><~+_-?][}{1)(|\\/tfjrxnumbroCLQZONWKDYswpkhBOXGAVUNFMEJ$@#";


int calculate_luminance(uint8_t r, uint8_t g, uint8_t b) {
    float luminance_float = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    return (int)luminance_float;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Please provide an image file path as an argument.\n");
        printf("Usage: %s <image_file_path>\n", argv[0]);
        return 1;
    }

    const char *image_path = argv[1];

    int width, height, channels;
    unsigned char *image_data = stbi_load(image_path, &width, &height, &channels, 0);

    if (image_data == NULL) {
        printf("Error: Could not load image '%s'.\n", image_path);
        return 1;
    }

    int num_ascii_chars = strlen(ASCII_CHARS);

    int x_step = 2; // Sample every 2nd pixel horizontally
    int y_step = 4; // Sample every 4th pixel vertically

    // Clamp step values to be at least 1 to avoid infinite loops or crashes
    if (x_step < 1) x_step = 1;
    if (y_step < 1) y_step = 1;

    // Iterate through the image pixels to generate ASCII art
    for (int y = 0; y < height; y += y_step) {
        for (int x = 0; x < width; x += x_step) {
            // Calculate the starting index of the pixel's data in the image_data array
            // The stride is `width * channels` bytes per row.
            int pixel_start_index = (y * width + x) * channels;

            // Define RGB values, initialized to 0
            uint8_t r = 0, g = 0, b = 0;

            // Extract RGB data based on the number of channels
            if (channels >= 3) { // RGB or RGBA
                r = image_data[pixel_start_index];
                g = image_data[pixel_start_index + 1];
                b = image_data[pixel_start_index + 2];
            } else if (channels == 1) { // Grayscale
                r = g = b = image_data[pixel_start_index];
            }
            // For other channel counts, the pixel will be black (r,g,b are 0)

            // Calculate the perceptual luminance
            int brightness = calculate_luminance(r, g, b);

            // Map the brightness value (0-255) to an index in the ASCII character set
            // The mapping scales the brightness range to the number of available characters.
            int char_index = (brightness * num_ascii_chars) / 256;

            // Safely ensure the index is within the bounds of the ASCII_CHARS string
            if (char_index < 0) char_index = 0;
            if (char_index >= num_ascii_chars) char_index = num_ascii_chars - 1;

            printf("%c", ASCII_CHARS[char_index]);
        }
        printf("\n"); 
    }

    //Free the memory allocated by stbi_load to prevent memory leaks
    stbi_image_free(image_data);

    return 0; 
}
