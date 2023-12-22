#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

const int SNAKE_START_X = 500;
const int SNAKE_START_Y = 500;
const int SEG_WIDTH = 25;
const int STEP_SIZE = SEG_WIDTH;
const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 700;
   
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
    if (distance <= SEG_WIDTH) {
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
    size_t snake_index = 0;
    SDL_Rect snake[200];
    SDL_Rect snake_last[200];
    snake[snake_index] = head;
    // snake_index += 1;
    // snake[snake_index] = create_segment(&snake[snake_index-1], dir);
    for (size_t i = 0; i <= snake_index; i++) {
        snake_last[i] = snake[i];
    }
    
    // initialize random food location
    int food_x = rand() % WINDOW_WIDTH;
    int food_y = rand() % WINDOW_HEIGHT;
    printf("Food: (%d, %d)\n", food_x, food_y);
    SDL_Rect food_rect = {food_x, food_y, SEG_WIDTH, SEG_WIDTH};

    Uint32 last_time = SDL_GetTicks();
    const Uint32 delta_time = 200;

    while (running) {
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_s) {
                    dir = DOWN;
                }
                else if (e.key.keysym.sym == SDLK_w) {
                    dir = UP;
                }
                else if (e.key.keysym.sym == SDLK_a) {
                    dir = LEFT;
                }
                else if (e.key.keysym.sym == SDLK_d) {
                    dir = RIGHT;
                }
                else if (e.key.keysym.sym == SDLK_q) {
                    running = false;
                }
            }
        }

        if (add_segment) {
            snake_index += 1;
            snake[snake_index] = create_segment(&snake[snake_index-1], dir);
            add_segment = false;
        }
        
        const Uint32 current_time = SDL_GetTicks();
        if ( (current_time - last_time) >= delta_time) {
            last_time = current_time;

            // move head
            switch (dir) {
                case DOWN:
                    snake[0].y += STEP_SIZE;
                    break;
                case UP:
                    snake[0].y -= STEP_SIZE;
                    break;
                case RIGHT:
                    snake[0].x += STEP_SIZE;
                    break;
                case LEFT:
                    snake[0].x -= STEP_SIZE;
                    break;
                case STOP:
                    break;
            }
            // each segment should move to where the one in front of it was
            for (size_t i = 1; i <= snake_index; i++) {
                snake[i] = snake_last[i-1];
            }
            // save this state
            for (size_t i = 0; i <= snake_index; i++) {
                snake_last[i] = snake[i];
            }
        }

        // Clear window
        SDL_RenderSetScale(renderer, 1.0, 1.0);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        

        // Draw body
        for (size_t i = 0; i <= snake_index; i++) {
            if (i == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            else if (i % 2 == 0){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }
            SDL_RenderFillRect(renderer, &snake[i]);
        }

        if (touching(&food_rect, &snake[0])) {
            add_segment = true;
            food_x = rand() % (WINDOW_WIDTH - SEG_WIDTH);
            food_y = rand() % (WINDOW_HEIGHT - SEG_WIDTH);
            food_rect.x = food_x;
            food_rect.y = food_y;
        }
        
        // printf("Length = %ld\n", snake_index + 1);

        // draw food
        SDL_SetRenderDrawColor(renderer, 210, 120, 10, 255);
        SDL_RenderFillRect(renderer, &food_rect);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(25);

    }

    return 0;

}
