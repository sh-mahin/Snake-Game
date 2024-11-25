#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <bits/stdc++.h>
#undef main

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
const int TILE_SIZE = 20;
int score = 0;
int bonusFoodTimer = 0;
bool pause = false;
bool gameOver = false;
bool quit = false;

class Snake {
public:
    Snake();
    void handleInput(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision();
    std::vector<SDL_Point> recentPositions;
    void spawnFood();
    void drawWall();

private:
    SDL_Rect food;
    std::vector<SDL_Rect> body;
    int direction; // 0 up, 1 down, 2 left, 3 right
    bool bonusFoodActive;
    SDL_Rect bonusFood;
};

Snake::Snake() {
    SDL_Rect head = {60, 60, TILE_SIZE, TILE_SIZE}; // Start position
    body.push_back(head);
    body.push_back({head.x, head.y, TILE_SIZE, TILE_SIZE});
    body.push_back({head.x, head.y, TILE_SIZE, TILE_SIZE});
    direction = 3; // Moving right initially
    spawnFood();
    bonusFoodActive = false;
}

void Snake::handleInput(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
        case SDLK_KP_8:
            if (direction != 1) direction = 0;
            pause = false;
            break;
        case SDLK_DOWN:
            if (direction != 0) direction = 1;
            pause = false;
            break;
        case SDLK_LEFT:
            if (direction != 3) direction = 2;
            pause = false;
            break;
        case SDLK_RIGHT:
            if (direction != 2) direction = 3;
            pause = false;
            break;
        case SDLK_SPACE:
            pause = !pause;
            break;
        case SDLK_0:
            quit = true;
            break;
        }
    }
}

void Snake::move() {
    SDL_Rect newHead = body.front();

    switch (direction) {
    case 0: newHead.y -= TILE_SIZE; break; // Up
    case 1: newHead.y += TILE_SIZE; break; // Down
    case 2: newHead.x -= TILE_SIZE; break; // Left
    case 3: newHead.x += TILE_SIZE; break; // Right
    }

    body.insert(body.begin(), newHead);

    if (newHead.x == food.x && newHead.y == food.y) {
        score += 1;
        spawnFood();
    } else {
        body.pop_back();
    }

    if (bonusFoodActive && newHead.x < bonusFood.x + bonusFood.w && newHead.x + newHead.w > bonusFood.x &&
        newHead.y < bonusFood.y + bonusFood.h && newHead.y + newHead.h > bonusFood.y) {
        score += 10;
        body.push_back(SDL_Rect{-20, -20, TILE_SIZE, TILE_SIZE});
        bonusFoodActive = false;
    }

    if (bonusFoodActive && SDL_GetTicks() > bonusFoodTimer) {
        bonusFoodActive = false;
    }

    if (checkCollision()) {
        gameOver = true;
    }
}

void Snake::render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
    for (int i = 1; i < body.size(); ++i)
        SDL_RenderFillRect(renderer, &body[i]);

    SDL_SetRenderDrawColor(renderer, 80, 120, 200, 255);
    SDL_RenderFillRect(renderer, &body[0]);

    SDL_SetRenderDrawColor(renderer, 255, 178, 102, 255);
    SDL_RenderFillRect(renderer, &food);

    if (bonusFoodActive) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &bonusFood);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect walls[] = {
        {SCREEN_WIDTH / 3 + 200, SCREEN_HEIGHT / 3 - 100, 20, 340},
        {SCREEN_HEIGHT / 3 + 160, SCREEN_WIDTH / 3 - 60, 340, 20},
        {100, 150, 20, 380},
        {1060 - 100, 150, 20, 380},
        {0, 720 - 4 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE * 4},
        {0, 0, 20, SCREEN_HEIGHT},
        {1060, 0, 20, SCREEN_HEIGHT},
        {0, 0, SCREEN_WIDTH, 20},
    };
    for (auto &wall : walls) SDL_RenderFillRect(renderer, &wall);
}

bool Snake::checkCollision() {
    SDL_Rect head = body.front();
    for (auto it = body.begin() + 1; it != body.end(); ++it)
        if (head.x == it->x && head.y == it->y) return true;

    if (head.y < 0 || head.y >= SCREEN_HEIGHT - 4 * TILE_SIZE ||
        head.x < TILE_SIZE || head.x >= SCREEN_WIDTH - TILE_SIZE) return true;

    SDL_Rect walls[] = {
        {SCREEN_WIDTH / 3 + 200, SCREEN_HEIGHT / 3 - 100, 20, 340},
        {SCREEN_HEIGHT / 3 + 160, SCREEN_WIDTH / 3 - 60, 340, 20},
        {100, 150, 20, 380},
        {1060 - 100, 150, 20, 380},
    };
    for (auto &wall : walls)
        if (head.x < wall.x + wall.w && head.x + head.w > wall.x &&
            head.y < wall.y + wall.h && head.y + head.h > wall.y)
            return true;

    return false;
}

void Snake::spawnFood() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> disX(20 / TILE_SIZE, 1060 / TILE_SIZE - 1);
    static std::uniform_int_distribution<> disY(60 / TILE_SIZE, 620 / TILE_SIZE - 1);

    SDL_Point foodPosition;
    do {
        foodPosition.x = disX(gen) * TILE_SIZE;
        foodPosition.y = disY(gen) * TILE_SIZE;
    } while (std::any_of(body.begin(), body.end(), [foodPosition](const SDL_Rect &segment) {
        return foodPosition.x == segment.x && foodPosition.y == segment.y;
    }));

    food = {foodPosition.x, foodPosition.y, TILE_SIZE, TILE_SIZE};

    if (score % 5 == 0) {
        bonusFoodActive = true;
        bonusFoodTimer = SDL_GetTicks() + 7000;
        SDL_Point bonusFoodPosition;
        do {
            bonusFoodPosition.x = disX(gen) * TILE_SIZE;
            bonusFoodPosition.y = disY(gen) * TILE_SIZE;
        } while (std::any_of(body.begin(), body.end(), [bonusFoodPosition](const SDL_Rect &segment) {
            return bonusFoodPosition.x == segment.x && bonusFoodPosition.y == segment.y;
        }));
        bonusFood = {bonusFoodPosition.x, bonusFoodPosition.y, TILE_SIZE, TILE_SIZE};
    }
}

void renderScore(SDL_Renderer *renderer, TTF_Font *font, int score) {
    SDL_Color fontColor = {255, 255, 102, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, ("Score: " + std::to_string(score)).c_str(), fontColor);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {800, 720 - 4 * TILE_SIZE, 7 * TILE_SIZE, 4 * TILE_SIZE};
    SDL_RenderCopy(renderer, text, nullptr, &textRect);
    SDL_DestroyTexture(text);
    SDL_FreeSurface(surface);
}

void displayGameOver(SDL_Renderer *renderer, TTF_Font *font, int finalScore) {
    SDL_SetRenderDrawColor(renderer, 204, 200, 153, 0);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, ("Game Over! Final Score: " + std::to_string(finalScore)).c_str(), textColor);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 5};
    SDL_RenderCopy(renderer, text, nullptr, &textRect);
    SDL_DestroyTexture(text);
    SDL_FreeSurface(surface);

    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
    exit(0);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Snake Game", SDL_WINDOW
