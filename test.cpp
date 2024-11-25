
#include <SDL2/SDL.h>
#include <deque>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;


const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 440;
const int TILE_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

class SnakeGame {
public:
    SnakeGame() : direction(RIGHT), snakeSize(1), score(0) {
        
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            exit(1);
        }

        
        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        
        snakeHead = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, TILE_SIZE, TILE_SIZE};
        snakeBody.push_back(snakeHead);

        
        srand(static_cast<unsigned>(time(0)));
        generateApple();
    }

    ~SnakeGame() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void run() {
        bool running = true;
        SDL_Event event;

        while (running) {
           
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN) {
                    handleDirection(event.key.keysym.sym);
                }
            }

           
            update();

            
            render();

            generateAppleWithTimer(4); // New apple every 5 seconds

            SDL_Delay(110); 
        }
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect snakeHead;
    deque<SDL_Rect> snakeBody;
    SDL_Rect apple;
    Direction direction;
    int snakeSize;
    int score;

    Uint32 lastAppleTime; 

    int getRandomCoord() {
        return (rand() % (WINDOW_WIDTH / TILE_SIZE)) * TILE_SIZE;
    }

    void handleDirection(SDL_Keycode key) {
        switch (key) {
            case SDLK_UP:
                if (direction != DOWN) direction = UP;
                break;
            case SDLK_DOWN:
                if (direction != UP) direction = DOWN;
                break;
            case SDLK_LEFT:
                if (direction != RIGHT) direction = LEFT;
                break;
            case SDLK_RIGHT:
                if (direction != LEFT) direction = RIGHT;
                break;
        }
    }

    void update() {
        
        switch (direction) {
            case UP:    snakeHead.y -= TILE_SIZE; break;
            case DOWN:  snakeHead.y += TILE_SIZE; break;
            case LEFT:  snakeHead.x -= TILE_SIZE; break;
            case RIGHT: snakeHead.x += TILE_SIZE; break;
        }

        

          if (snakeHead.x < 0 || snakeHead.y < 0 || 
            snakeHead.x >= WINDOW_WIDTH || snakeHead.y >= WINDOW_HEIGHT) {
            gameOver(); // End the game if snake hits the wall
            return;
            }
        // 
    //      if (snakeHead.x < 0)
    // {
    //     snakeHead.x =WINDOW_WIDTH ;
    // }
    // else if (snakeHead.x >= WINDOW_WIDTH)
    // {
    //     snakeHead.x = 0;
    // }
    //        if (snakeHead.y < 0)
    // {
    //     snakeHead.y =WINDOW_HEIGHT ;
    // }
    // else if (snakeHead.y >= WINDOW_HEIGHT)
    // {
    //     snakeHead.y = 0;
    // }


        for (const auto& segment : snakeBody) {
            if (snakeHead.x == segment.x && snakeHead.y == segment.y) {
                
                gameOver();// End the game if snake hits itself
            
                return;
            }
        }

        if (snakeHead.x == apple.x && snakeHead.y == apple.y) {
            score++; 
            snakeSize++; 
            generateApple(); 
        }

       
        snakeBody.push_front(snakeHead); 
        while (snakeBody.size() > static_cast<size_t>(snakeSize)) {
            snakeBody.pop_back();
        }
    }

    void render() {
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);

       
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &apple);

        
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        for (const auto& segment : snakeBody) {
            SDL_RenderFillRect(renderer, &segment);
        }

        
        SDL_RenderPresent(renderer);
    }
    

    void generateApple() {
        apple = {getRandomCoord(), getRandomCoord(), TILE_SIZE, TILE_SIZE}; // Generate new apple at random position
        lastAppleTime = SDL_GetTicks(); 
    }

      void generateAppleWithTimer(int timeLimit) {
        Uint32 currentTime = SDL_GetTicks();
        if ((currentTime - lastAppleTime) >= (timeLimit * 1000)) {
            generateApple();
        }
    }

     void gameOver() {
        cout << "Game Over! Your Score: " << score << endl;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(1500);
        resetGame();
    }

    void resetGame() {
       
        snakeHead = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, TILE_SIZE, TILE_SIZE};
        snakeBody.clear();
        snakeBody.push_back(snakeHead);
        snakeSize = 1;
        score = 0; 
        direction = RIGHT;
        generateApple();
    }
};


int SDL_main(int argc, char* argv[]) {
    SnakeGame game;
    game.run();
    return 0;
}
