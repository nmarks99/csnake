#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h> 
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Surface *screen_surface = NULL;
SDL_Surface *hello_world = NULL;

bool init_graphics() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL\n");
        success = false;
    }
    else {
        window = SDL_CreateWindow(
            "window name",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );
        if (window == NULL) {
            printf("Could not create window\n");
            success = false;
        }
        else {
            screen_surface = SDL_GetWindowSurface(window);
        }
    }
    return success;
}

bool load_media(const char* img_path) {
    bool success = true;
    hello_world = SDL_LoadBMP(img_path);
    if (hello_world == NULL) {
        success = false;
    }
    return success;
}

// void close() {
//     SDL_FreeSurface(hello_world);
//     hello_world = NULL;
//     SDL_DestroyWindow(window);
//     window = NULL;
//     SDL_Quit();
// }

int main(int argc, char* args[]) {
    if (!init_graphics()) {
        printf("failed\n");
    }
    else {
        if (!load_media("./assets/rocket.bmp")) {
            printf("Failed to load image\n");
        }
        else {
            SDL_BlitSurface(hello_world, NULL, screen_surface, NULL);
            SDL_UpdateWindowSurface(window);
            SDL_Delay(2000);
        }
    }
    
    SDL_FreeSurface(hello_world);
    hello_world = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();

    return 0;
}
