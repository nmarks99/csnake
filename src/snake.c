#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

typedef struct Text {
    const char *msg;
    SDL_Color color;
    int x;
    int y;
} Text; 

const int SEG_WIDTH = 30;
const int STEP_SIZE = SEG_WIDTH;
const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 700;
const int SNAKE_START_X = WINDOW_WIDTH/2;
const int SNAKE_START_Y = WINDOW_HEIGHT/2;
const int WINDOW_ALPHA = 100;
const int SNAKE_BUFFER_SIZE = 500;
const bool WRAP = false;
const char FONT_PATH[] = "./fonts/JetBrainsMonoNerdFont-Medium.ttf";
const int DEFAULT_FONT_SIZE = 32;

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


SDL_Rect create_food(const SDL_Rect *snake, int snake_index) {
    while (true) {
        const int food_x = rand() % (WINDOW_WIDTH - SEG_WIDTH);
        const int food_y = rand() % (WINDOW_HEIGHT - SEG_WIDTH);
        const SDL_Rect food_rect = {food_x, food_y, SEG_WIDTH, SEG_WIDTH};

        bool overlap = false;
        for (int i = 1; i < snake_index; i++) {
            if (touching(&snake[i], &food_rect, SEG_WIDTH)) {
                overlap = true;
                break;
            }
        }
        if (!overlap) {
            return food_rect;
        }
    }
}

bool collision_detected(const SDL_Rect *snake, int snake_index) {
    for (int i = 1; i <= snake_index; i++) {
        if (touching(&snake[0], &snake[i], SEG_WIDTH/2)) {
            return true;
        }
    }
    return false;
}

void save_snake_state(const SDL_Rect *snake, SDL_Rect *snake_save, int snake_index) {
    for (int i = 0; i <= snake_index; i++) {
        snake_save[i] = snake[i];
    }
}

void draw_text(Text text, TTF_Font *font, SDL_Renderer *renderer) {
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text.msg, text.color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect dest = {text.x, text.y, text_surface->w, text_surface->h};
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, text_texture, NULL, &dest);
    SDL_RenderPresent(renderer);
}

void show_score(int snake_index,TTF_Font *font, SDL_Renderer *renderer){
    
    SDL_SetRenderDrawColorCommon(renderer, BLACK, WINDOW_ALPHA);
    SDL_RenderClear(renderer);

    char buff[50];
    sprintf(buff, "Score: %d", snake_index + 1);
    Text text = {
        .msg = buff,
        .x = 0,
        .y = 0,
        .color = {255, 255, 255}
    };
    draw_text(text, font, renderer);
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
    
    if (TTF_Init() < 0) {
        printf("Failed to initialize TTF\n");
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColorCommon(renderer, BLACK, WINDOW_ALPHA);
    SDL_RenderClear(renderer);
    
    TTF_Font* font;
    font = TTF_OpenFont(FONT_PATH, DEFAULT_FONT_SIZE);
    if (!font) {
        fprintf(stderr, "Font loading failed: %s\n", TTF_GetError());
        return 1;
    }
    
    Text text = {
        .msg = "Snake",
        .x = WINDOW_WIDTH/2,
        .y = WINDOW_HEIGHT/2,
        .color = {255, 255, 255}
    };
    draw_text(text, font, renderer);
    SDL_Delay(1000);

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
    SDL_Rect snake[SNAKE_BUFFER_SIZE];
    SDL_Rect snake_last[SNAKE_BUFFER_SIZE];
    snake[snake_index] = head;
    save_snake_state(snake, snake_last, snake_index);

    SDL_Rect food = create_food(snake, snake_index);

    Uint32 last_time = SDL_GetTicks();
    const Uint32 delta_time = 150;

    while (running) {
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                show_score(snake_index, font, renderer);
                SDL_Delay(2000);
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

            // enable wrap-around
            if (WRAP) {
                if (snake[0].x < 0) {
                    snake[0].x = WINDOW_WIDTH;
                }
                else if (snake[0].x > WINDOW_WIDTH) {
                    snake[0].x = 0;
                }
                else if (snake[0].y > WINDOW_HEIGHT) {
                    snake[0].y = 0;
                }
                else if (snake[0].y < 0) {
                    snake[0].y = WINDOW_HEIGHT;
                }
            }
            else {
                if (snake[0].x < 0 || snake[0].x > WINDOW_WIDTH) {
                    dead = true;
                }
                if (snake[0].y < 0 || snake[0].y > WINDOW_WIDTH) {
                    dead = true;
                }
            }

            // each segment should move to where the one in front of it was
            for (int i = 1; i <= snake_index; i++) {
                snake[i] = snake_last[i-1];
            }

            save_snake_state(snake, snake_last, snake_index);
        }

        if (touching(&food, &snake[0], SEG_WIDTH)) {
            add_segment = true;
            food = create_food(snake, snake_index);
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
        }

        // You lost, flicker snake, show score, and exit
        else {
            flash_snake(snake, renderer, snake_index);
            show_score(snake_index, font, renderer);
            SDL_Delay(2000);
            running = false;
        }

    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    return 0;
}
