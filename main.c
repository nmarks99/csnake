#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

const int STEP_SIZE = 5;
const int SNAKE_START_X = 500;
const int SNAKE_START_Y = 500;
const int SEG_WIDTH = 20;
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
   
enum Direction {
    DOWN,
    LEFT,
    RIGHT,
    UP,
    STOP
};

bool touching(const SDL_Rect *r1, const SDL_Rect *r2) {
    const int x1 = r1->x;
    const int y1 = r1->y;
    const int x2 = r2->x;
    const int y2 = r2->y;
    const double distance = sqrt( pow((x2 - x1), 2) + pow((y2 - y1),2) );
    if (distance <= 20.0) {
        return true;
    }
    else {
        return false;
    }
}


SDL_Rect create_segment(const SDL_Rect *rect, int direction) {
    
    int segx = rect->x;
    int segy = rect->y;
    const int segw = rect->w;
    const int segh = rect->h;
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
    const SDL_Rect seg = {
        .x = segx,
        .y = segy,
        .w = segw,
        .h = segh
    };

    return seg;
}


int main() {

    srand(time(NULL));

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "Snake",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Event e;
    bool running = true;

    SDL_Rect head = {
        .x = SNAKE_START_X,
        .y = SNAKE_START_Y,
        .w = SEG_WIDTH,
        .h = SEG_WIDTH
    };

    int dir = STOP;
    bool add_segment = false;
    size_t body_index = 0;
    SDL_Rect body[200];
    SDL_Rect body_last[200];
    body[body_index] = head;
    body_index += 1;
    body[body_index] = create_segment(&body[body_index-1], dir);
    for (size_t i = 0; i <= body_index; i++) {
        body_last[i] = body[i];
    }

    int food_x = rand() % WINDOW_WIDTH;
    int food_y = rand() % WINDOW_WIDTH;
    SDL_Rect food_rect = {food_x, food_y, SEG_WIDTH, SEG_WIDTH};

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
            else if (i % 2 == 0){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }
            SDL_RenderFillRect(renderer, &body[i]);
        }

        if (touching(&food_rect, &body[0])) {
            add_segment = true;
            food_x = rand() % 1001;
            food_y = rand() % 1001;
            food_rect.x = food_x;
            food_rect.y = food_y;
        }
        
        printf("Length = %ld\n", body_index + 1);

        // draw food
        SDL_SetRenderDrawColor(renderer, 210, 120, 10, 255);
        SDL_RenderFillRect(renderer, &food_rect);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(25);

    }

    return 0;

}
