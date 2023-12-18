#include <SDL2/SDL.h>
#include <stdbool.h>

const int STEP_SIZE = 5;
const int SNAKE_START_X = 500;
const int SNAKE_START_Y = 500;
const int SNAKE_WIDTH = 10;
const int SNAKE_HEIGHT = 10;


   
enum Direction {
    DOWN,
    LEFT,
    RIGHT,
    UP,
    STOP
};


SDL_Rect create_segment(const SDL_Rect *snake_head, int direction) {
    
    int segx = snake_head->x;
    int segy = snake_head->y;
    const int segw = snake_head->w;
    const int segh = snake_head->h;
    switch (direction) {
        case (DOWN):
            segy -= segw; 
            break;
        case (UP):
            segy += segw;
            break;
        case (RIGHT):
            segx -= segw;
            break;
        case (LEFT):
            segx += segw;
            break;
        default:
            segx += segw;
            break;
            
    }
    SDL_Rect seg = {
        .x = segx,
        .y = segy,
        .w = segw,
        .h = segh
    };

    return seg;
}



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

    int dir = 0;

    SDL_Rect head = {
        .x = SNAKE_START_X,
        .y = SNAKE_START_Y,
        .w = SNAKE_WIDTH,
        .h = SNAKE_HEIGHT
    };

    dir = STOP;
    bool add_segment = false;
    bool flipflop = false;
    size_t body_index = 0;
    SDL_Rect body[200];
    SDL_Rect body_last[200];
    body[body_index] = head;
    body_index += 1;
    body[body_index] = create_segment(&body[body_index-1], dir);
    for (size_t i = 0; i <= body_index; i++) {
        body_last[i] = body[i];
    }
    while (running) {

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
                if (e.key.keysym.sym == SDLK_a) {
                    add_segment = true;
                }
            }
        }

        if (add_segment) {
            body_index += 1;
            body[body_index] = create_segment(&body[body_index-1], dir);
            add_segment = false;
        }

        // move head
        switch (dir) {
            case DOWN:
                body[0].y += STEP_SIZE;
                break;
            case UP:
                body[0].y -= STEP_SIZE;
                break;
            case RIGHT:
                body[0].x += STEP_SIZE;
                break;
            case LEFT:
                body[0].x -= STEP_SIZE;
                break;
            case STOP:
                break;
        }
        
        // each segment should move to where the one in front of it was
        for (size_t i = 1; i <= body_index; i++) {
            body[i].x = body_last[i-1].x;
            body[i].y = body_last[i-1].y;
        }

        // save this state
        for (size_t i = 0; i <= body_index; i++) {
            body_last[i] = body[i];
        }

        // Clear window
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw body
        for (size_t i = 0; i <= body_index; i++) {
            if (i == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            SDL_RenderFillRect(renderer, &body[i]);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(25);

    }

    return 0;

}
