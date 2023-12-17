#include <SDL2/SDL.h>
#include <stdbool.h>

const int STEP_SIZE = 5;
const int SNAKE_START_X = 500;
const int SNAKE_START_Y = 500;
const int SNAKE_WIDTH = 10;
const int SNAKE_HEIGHT = 10;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "Snake",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1000,
        1000,
        0
    );
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Event e;
    bool running = true;
   
    enum Direction {
        DOWN,
        LEFT,
        RIGHT,
        UP,
        STOP
    };

    int dir = 0;

    SDL_Rect head = {
        .x = SNAKE_START_X,
        .y = SNAKE_START_Y,
        .w = SNAKE_WIDTH,
        .h = SNAKE_HEIGHT
    };

    SDL_Rect body = {
        .x = SNAKE_START_X+SNAKE_WIDTH,
        .y = SNAKE_START_Y,
        .w = SNAKE_WIDTH,
        .h = SNAKE_HEIGHT
    };
    dir = STOP;
    while (running) {

        // Check input
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_DOWN) {
                    dir = DOWN;
                }
                if (e.key.keysym.sym == SDLK_UP) {
                    dir = UP;
                }
                if (e.key.keysym.sym == SDLK_LEFT) {
                    dir = LEFT;
                }
                if (e.key.keysym.sym == SDLK_RIGHT) {
                    dir = RIGHT;
                }
            }
            else if (e.type == SDL_KEYUP) {
                dir = STOP;
            }
        }

        // Move
        switch (dir) {
            case DOWN:
                head.y += STEP_SIZE;
                body.y += STEP_SIZE;
                break;
            case UP:
                head.y -= STEP_SIZE;
                body.y -= STEP_SIZE;
                break;
            case RIGHT:
                head.x += STEP_SIZE;
                body.x += STEP_SIZE;
                break;
            case LEFT:
                head.x -= STEP_SIZE;
                body.x -= STEP_SIZE;
                break;
            case STOP:
                break;
        }

        // Clear window
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw body
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &head);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &body);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(25);

    }

    return 0;

}
