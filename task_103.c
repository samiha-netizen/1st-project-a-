#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#define FALSE 0
#define TRUE 1

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int game_is_running = FALSE;

int circle1_x = 0;
int circle1_y = WINDOW_HEIGHT / 2;
int radius1 = 40;

int circle2_x = WINDOW_WIDTH / 2;
int circle2_y = 0;
int radius2 = 40;

int collision = FALSE;

int initialize_window()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL Error: %s\n", SDL_GetError());
        return FALSE;
    }

    window = SDL_CreateWindow(
        "Task 103",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    return TRUE;
}

void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{
    for(int w = 0; w < radius * 2; w++)
    {
        for(int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w;
            int dy = radius - h;

            if(dx * dx + dy * dy <= radius * radius)
            {
                SDL_RenderDrawPoint(
                    renderer,
                    centerX + dx,
                    centerY + dy
                );
            }
        }
    }
}

void process_input()
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            game_is_running = FALSE;

        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_UP:
                    circle2_y -= 10;
                    break;

                case SDLK_DOWN:
                    circle2_y += 10;
                    break;

                case SDLK_LEFT:
                    circle2_x -= 10;
                    break;

                case SDLK_RIGHT:
                    circle2_x += 10;
                    break;

                case SDLK_ESCAPE:
                    game_is_running = FALSE;
                    break;
            }
        }
    }
}

void update()
{
    circle1_x += 3;

    if(circle1_x > WINDOW_WIDTH + radius1)
    {
        circle1_x = -radius1;
    }

    double distance = sqrt(
        (circle2_x - circle1_x) * (circle2_x - circle1_x) +
        (circle2_y - circle1_y) * (circle2_y - circle1_y)
    );

    collision = FALSE;

    if(distance <= radius1 + radius2)
    {
        collision = TRUE;
    }
}

void draw()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if(collision)
    {
        /* Both become RED on collision */
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        drawCircle(renderer, circle1_x, circle1_y, radius1);
        drawCircle(renderer, circle2_x, circle2_y, radius2);
    }
    else
    {
        /* Circle 1 = Blue */
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        drawCircle(renderer, circle1_x, circle1_y, radius1);

        /* Circle 2 = Green */
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        drawCircle(renderer, circle2_x, circle2_y, radius2);
    }

    SDL_RenderPresent(renderer);
}

void destroy_window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    game_is_running = initialize_window();

    while(game_is_running)
    {
        process_input();
        update();
        draw();

        SDL_Delay(16);
    }

    destroy_window();

    return 0;
}