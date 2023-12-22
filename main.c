#include <SDL2/SDL.h>
// #include <SDL2/SDL_render.h>
// #include <SDL2/SDL_stdinc.h>
// #include <SDL2/SDL_video.h>
#include <SDL2/SDL_keycode.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

   
enum Direction {
    DOWN,
    LEFT,
    RIGHT,
    UP,
    STOP
};

typedef enum DrawColor {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    PURPLE,
    BLACK,
    WHITE
} DrawColor;

const int SNAKE_START_X = 500;
const int SNAKE_START_Y = 500;
const int SEG_WIDTH = 25;
const int STEP_SIZE = SEG_WIDTH;
const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 700;
const int WINDOW_ALPHA = 100;

bool touching(const SDL_Rect *r1, const SDL_Rect *r2, int tol) {
    const int x1 = r1->x;
    const int y1 = r1->y;
    const int x2 = r2->x;
    const int y2 = r2->y;
    const double distance = sqrt( pow((x2 - x1), 2) + pow((y2 - y1),2) );
    if (distance <= tol) {
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



void SDL_SetRenderDrawColorCommon(SDL_Renderer *renderer, DrawColor color, int alpha) {
    int r;
    int g;
    int b;
    switch (color) {
        case (RED):
            r = 255; g = 0; b = 0;
            break;
        case (ORANGE):
            r = 255; g = 165; b = 0;
            break;
        case (YELLOW):
            r = 255; g = 255; b = 0;
            break;
        case (GREEN):
            r = 0; g = 128; b = 0;
            break;
        case (BLUE):
            r = 0; g = 0; b = 255;
            break;
        case (PURPLE):
            r = 128; g = 0; b = 128;
            break;
        case (WHITE):
            r = 255; g = 255; b = 255;
            break;
        case (BLACK):
            r = 0; g = 0; b = 0;
            break;
    }
    
    alpha = (alpha > 100) ? 100 : alpha;
    alpha = (alpha < 0) ? 0 : alpha;
    alpha = (int)(((double)alpha / 100.0) * 255.0);
    SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}


void render_snake(SDL_Rect *snake, SDL_Renderer *renderer, int snake_index, int snake_opacity) {
    for (int i = 0; i <= snake_index; i++) {
        if (i % 2 == 0){
            SDL_SetRenderDrawColorCommon(renderer, YELLOW, snake_opacity);
        }
        else {
            SDL_SetRenderDrawColorCommon(renderer, RED, snake_opacity);
        }
        SDL_RenderFillRect(renderer, &snake[i]);
    }
}

void flash_snake(SDL_Rect *snake, SDL_Renderer *renderer, int snake_index) {

    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            render_snake(snake, renderer, snake_index, 100);
            SDL_RenderPresent(renderer);
            SDL_Delay(100);
        }
        else {
            render_snake(snake, renderer, snake_index, 10);
            SDL_RenderPresent(renderer);
            SDL_Delay(100);
        }
        SDL_SetRenderDrawColorCommon(renderer, BLACK, WINDOW_ALPHA);
        SDL_RenderClear(renderer);
    }
}


SDL_Rect create_food() {
    const int food_x = rand() % (WINDOW_WIDTH - SEG_WIDTH);
    const int food_y = rand() % (WINDOW_HEIGHT - SEG_WIDTH);
    const SDL_Rect food_rect = {food_x, food_y, SEG_WIDTH, SEG_WIDTH};
    return food_rect;
}

bool collision_detected(SDL_Rect *snake, int snake_index) {
    for (int i = 1; i <= snake_index; i++) {
        if (touching(&snake[0], &snake[i], SEG_WIDTH/2)) {
            return true;
        }
    }
    return false;
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
        SDL_WINDOW_SHOWN
    );
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Event e;
    bool running = true;

    SDL_Rect head = {
        .x = SNAKE_START_X,
        .y = SNAKE_START_Y,
        .w = SEG_WIDTH,
        .h = SEG_WIDTH
    };

    bool dead = false;
    int dir = STOP;
    bool add_segment = false;
    int snake_index = 0;
    SDL_Rect snake[200];
    SDL_Rect snake_last[200];
    snake[snake_index] = head;
    // snake_index += 1;
    // snake[snake_index] = create_segment(&snake[snake_index-1], dir);
    for (int i = 0; i <= snake_index; i++) {
        snake_last[i] = snake[i];
    }
    
    // initialize random food location
    SDL_Rect food = create_food();

    Uint32 last_time = SDL_GetTicks();
    const Uint32 delta_time = 150;


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
                else if (e.key.keysym.sym == SDLK_SPACE) {
                    dir = STOP;
                }
                else if (e.key.keysym.sym == SDLK_y) {
                    add_segment = true;
                }
            }
        }

        if (add_segment) {
            snake_index += 1;
            snake[snake_index] = create_segment(&snake[snake_index-1], dir);
            add_segment = false;
        }
        
        const Uint32 current_time = SDL_GetTicks();
        if ( ((current_time - last_time) >= delta_time) && dir != STOP) {
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
            for (int i = 1; i <= snake_index; i++) {
                snake[i] = snake_last[i-1];
            }
            // save this state
            for (int i = 0; i <= snake_index; i++) {
                snake_last[i] = snake[i];
            }
        }

        if (touching(&food, &snake[0], SEG_WIDTH)) {
            add_segment = true;
            food = create_food();
        }
        
        if (collision_detected(snake, snake_index)) { dead = true; }
        
        // Clear window
        SDL_SetRenderDrawColorCommon(renderer, BLACK, WINDOW_ALPHA);
        SDL_RenderClear(renderer);

        if (!dead) {
            render_snake(snake, renderer, snake_index, 100);

            SDL_SetRenderDrawColorCommon(renderer, GREEN, 100);
            SDL_RenderFillRect(renderer, &food);
            
            SDL_RenderPresent(renderer);
            SDL_Delay(25);
        }

        // You lost, flicker snake and exit
        else {
            flash_snake(snake, renderer, snake_index);
            running = false;
        }

    }
    return 0;
}
