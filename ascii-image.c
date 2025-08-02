#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <time.h> 
#include <dirent.h> 

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"


const char *ASCII_CHARS = " .'`^,:;Il!i><~+_-?][}{1)(|\\/tfjrxnumbroCLQZONWKDYswpkhBOXGAVUNFMEJ$@#";


int calculate_luminance(uint8_t r, uint8_t g, uint8_t b) {
    float luminance_float = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    return (int)luminance_float;
}

int is_image_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    
    char lower_ext[5];
    strncpy(lower_ext, ext, 4);
    lower_ext[4] = '\0';
    
    for (int i = 0; i < strlen(lower_ext); i++) {
        if (lower_ext[i] >= 'A' && lower_ext[i] <= 'Z') {
            lower_ext[i] += 32;
        }
    }

    if (strcmp(lower_ext, ".jpg") == 0 ||
        strcmp(lower_ext, ".jpeg") == 0 ||
        strcmp(lower_ext, ".png") == 0 ||
        strcmp(lower_ext, ".bmp") == 0) {
        return 1;
    }
    return 0;
}


int main() {
    const char *image_folder_path = "./images";
    
    DIR *dir;
    struct dirent *ent;
    char **image_files = NULL;
    int num_images = 0;
    int max_images = 10;

    image_files = (char**)malloc(sizeof(char*) * max_images);
    if (image_files == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for image file list.\n");
        return 1;
    }

    if ((dir = opendir(image_folder_path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && is_image_file(ent->d_name)) {
                if (num_images >= max_images) {
                    max_images *= 2;
                    image_files = (char**)realloc(image_files, sizeof(char*) * max_images);
                    if (image_files == NULL) {
                        fprintf(stderr, "Error: Could not reallocate memory for image file list.\n");
                        closedir(dir);
                        return 1;
                    }
                }
                image_files[num_images] = (char*)malloc(strlen(ent->d_name) + 1);
                if (image_files[num_images] == NULL) {
                    fprintf(stderr, "Error: Could not allocate memory for filename.\n");
                    closedir(dir);
                    return 1;
                }
                strcpy(image_files[num_images], ent->d_name);
                num_images++;
            }
        }
        closedir(dir);
    } else {
        fprintf(stderr, "Error: Could not open directory '%s'.\n", image_folder_path);
        free(image_files);
        return 1;
    }
    
    if (num_images == 0) {
        fprintf(stderr, "Error: No image files found in directory '%s'.\n", image_folder_path);
        free(image_files);
        return 1;
    }

    srand(time(NULL)); 
    int random_index = rand() % num_images;
    char *random_filename = image_files[random_index];

    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", image_folder_path, random_filename);

    printf("Loading random image: %s\n", full_path);

    int width, height, channels;
    unsigned char *image_data = stbi_load(full_path, &width, &height, &channels, 0);

    if (image_data == NULL) {
        fprintf(stderr, "Error: Could not load image '%s'.\n", full_path);
        for (int i = 0; i < num_images; i++) {
            free(image_files[i]);
        }
        free(image_files);
        return 1;
    }

    int num_ascii_chars = strlen(ASCII_CHARS);
    
    int x_step = 2;
    int y_step = 4;

    if (x_step < 1) x_step = 1;
    if (y_step < 1) y_step = 1;

    for (int y = 0; y < height; y += y_step) {
        for (int x = 0; x < width; x += x_step) {
            int pixel_start_index = (y * width + x) * channels;

            uint8_t r = 0, g = 0, b = 0;

            if (channels >= 3) {
                r = image_data[pixel_start_index];
                g = image_data[pixel_start_index + 1];
                b = image_data[pixel_start_index + 2];
            } else if (channels == 1) {
                r = g = b = image_data[pixel_start_index];
            }
            
            int brightness = calculate_luminance(r, g, b);
            int char_index = (brightness * num_ascii_chars) / 256;

            if (char_index < 0) char_index = 0;
            if (char_index >= num_ascii_chars) char_index = num_ascii_chars - 1;

            printf("\033[38;2;%d;%d;%dm%c\033[0m", r, g, b, ASCII_CHARS[char_index]);
        }
        printf("\n"); 
    }

    stbi_image_free(image_data);

    for (int i = 0; i < num_images; i++) {
        free(image_files[i]);
    }
    free(image_files);

    return 0; 
}
