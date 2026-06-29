#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h> // Added for sprintf to format the score text

// --- Game Constants ---
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20
#define MAX_SNAKE_LENGTH 100

// --- Direction Enum ---
typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// --- Structure for a 2D Point ---
typedef struct
{
    int x, y;
} Point;

// --- Main Game Variables ---
Point snake[MAX_SNAKE_LENGTH];
int snake_length = 3;
Direction current_dir = DIR_RIGHT;
Direction next_dir = DIR_RIGHT;

Point food;
int score = 0;
bool game_running = true;
bool game_over = false;

// --- Function to spawn food at a random grid position ---
void spawn_food()
{
    food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
    food.y = (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;
}

// --- Initialize / Reset the Snake ---
void init_game()
{
    srand((unsigned int)time(NULL));

    snake_length = 3;
    snake[0] = (Point){320, 240};
    snake[1] = (Point){300, 240};
    snake[2] = (Point){280, 240};

    current_dir = DIR_RIGHT;
    next_dir = DIR_RIGHT;
    score = 0;
    game_over = false;

    spawn_food();
}

// --- Show Game Over Message Box ---
void show_game_over_box(SDL_Window *window)
{
    char message_text[100];
    // Create a custom message string containing the final score
    sprintf(message_text, "Game Over!\nYour Final Score: %d\n\nWould you like to play again?", score);

    // Set up a structural message box with buttons
    const SDL_MessageBoxButtonData buttons[] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Play Again"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Quit"},
    };

    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        window,
        "SNAKE GAME",
        message_text,
        SDL_arraysize(buttons),
        buttons,
        NULL};

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
    {
        // Fallback option if custom message box fails
        game_running = false;
        return;
    }

    if (buttonid == 1)
    {
        init_game(); // Restart the game
    }
    else
    {
        game_running = false; // Quit the game loop
    }
}

// --- Step 1: Input Handling ---
void handle_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            game_running = false;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                if (current_dir != DIR_DOWN)
                    next_dir = DIR_UP;
                break;
            case SDLK_DOWN:
                if (current_dir != DIR_UP)
                    next_dir = DIR_DOWN;
                break;
            case SDLK_LEFT:
                if (current_dir != DIR_RIGHT)
                    next_dir = DIR_LEFT;
                break;
            case SDLK_RIGHT:
                if (current_dir != DIR_LEFT)
                    next_dir = DIR_RIGHT;
                break;
            }
        }
    }
}

// --- Step 2: Game Logic Update ---
void update_game(SDL_Window *window)
{
    if (game_over)
        return;

    current_dir = next_dir;

    // Move the body
    for (int i = snake_length - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }

    // Move the head
    switch (current_dir)
    {
    case DIR_UP:
        snake[0].y -= GRID_SIZE;
        break;
    case DIR_DOWN:
        snake[0].y += GRID_SIZE;
        break;
    case DIR_LEFT:
        snake[0].x -= GRID_SIZE;
        break;
    case DIR_RIGHT:
        snake[0].x += GRID_SIZE;
        break;
    }

    // Collision: Boundaries
    if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH ||
        snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT)
    {
        game_over = true;
    }

    // Collision: Own body
    for (int i = 1; i < snake_length; i++)
    {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
        {
            game_over = true;
        }
    }

    // Collision: Food
    if (snake[0].x == food.x && snake[0].y == food.y)
    {
        score += 10;
        if (snake_length < MAX_SNAKE_LENGTH)
        {
            snake_length++;
        }
        spawn_food();
    }

    // If a collision happened during this frame, trigger the option window immediately
    if (game_over)
    {
        show_game_over_box(window);
    }
}

// --- Step 3: Rendering Graphics ---
void render_game(SDL_Renderer *renderer)
{
    // Clear screen with a Dark Gray background
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Draw Food (Red Square)
    SDL_SetRenderDrawColor(renderer, 231, 76, 60, 255);
    SDL_Rect food_rect = {food.x, food.y, GRID_SIZE - 2, GRID_SIZE - 2};
    SDL_RenderFillRect(renderer, &food_rect);

    // Draw Snake (Green Squares)
    for (int i = 0; i < snake_length; i++)
    {
        if (i == 0)
        {
            SDL_SetRenderDrawColor(renderer, 46, 204, 113, 255); // Bright Green Head
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 39, 174, 96, 255); // Darker Green Body
        }
        SDL_Rect body_rect = {snake[i].x, snake[i].y, GRID_SIZE - 2, GRID_SIZE - 2};
        SDL_RenderFillRect(renderer, &body_rect);
    }

    SDL_RenderPresent(renderer);
}

// --- Main Entry Point ---
int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Simple Snake Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window)
    {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    init_game();

    // Main Game Loop
    while (game_running)
    {
        handle_input();
        update_game(window); // Passed the window pointer to handle the pop-up location
        render_game(renderer);

        // CHANGED: Delay increased from 100 to 150. Higher delay = slower movement speed.
        SDL_Delay(150);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}