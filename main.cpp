#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

void jump(SDL_Rect& rect, int v0, int y0, int t);

constexpr int window_w = 800;
constexpr int window_h = 600;
constexpr int FPS = 60;
constexpr int frameDelay = 1000 / FPS;

struct TextureStructure
{
    int offset;
    int count;
};

int main(int argc, char* argv[])
{
    bool isRunning = true;
    Uint32 frameStart, frameTime, frameCount = 0;
    Uint32 format;
    int accessTex, wTex, hTex;
    int wDino = 24, hDino = 24;
    TextureStructure dinoState[6]{ {0, 4}, {wDino * 4, 6}, {wDino * 10, 3}, {wDino * 13, 4}, {wDino * 17, 1}, {wDino * 18, 6} };
    int dinoStateIdx = 0;
    int dinoActionIdx = 0;
    bool dinoWalk = false;
    bool dinoRun = false;
    bool dinoJump = false;
    bool dinoJumpLock = false;
    int dinoSpeed = 0;
    int dinoSpeedLock = 0;
    int dinoJumpHeight = 0;
    int hGround = 310;
    int jumpframe = 0;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow(
        "Dino", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_w, window_h, false);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Event event;

    // Texture
    SDL_Surface* tmpSurface;

    tmpSurface = IMG_Load("assets/background.png");
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    tmpSurface = IMG_Load("assets/DinoSprites - vita.png");
    SDL_Texture* dinoTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);

    SDL_FreeSurface(tmpSurface);
    // End of Texture

    // Rectangle
    SDL_Rect bgRectSrc{ 0, 0, window_w, window_h };
    SDL_Rect bgRectDes{ 0, 0, window_w, window_h };
    SDL_Rect dinoRectSrc{ 0, 0, wDino, hDino };
    SDL_Rect dinoRectDes{ 50, hGround, wDino*10, hDino*10 };
    



    // Main Loop
    while (isRunning)
    {
        // Get Ticks
        frameStart = (Uint32)SDL_GetTicks64();

        // Get Event
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;

        /* Keyboard event */
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                isRunning = false;
                break;
            case SDLK_1:
                dinoStateIdx = 0;
                dinoActionIdx = 0;
                break;
            case SDLK_2:
                dinoStateIdx = 1;
                dinoActionIdx = 0;
                break;
            case SDLK_3:
                dinoStateIdx = 2;
                dinoActionIdx = 0;
                break;
            case SDLK_4:
                dinoStateIdx = 3;
                dinoActionIdx = 0;
                break;
            case SDLK_5:
                dinoStateIdx = 4;
                dinoActionIdx = 0;
                break;
            case SDLK_6:
                dinoStateIdx = 5;
                dinoActionIdx = 0;
                break;
            case SDLK_RIGHT:
                dinoRun = true;
                break;
            case SDLK_RETURN:
                dinoWalk = true;
                break;
            case SDLK_SPACE:
                if (dinoWalk)
                {
                    dinoJump = true;
                }
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHT:
                dinoRun = false;
                break;
            }
            break;
        }

        
        // dino 애니메이션
        if (frameCount % 3 == 0)
        {
            if (dinoActionIdx < dinoState[dinoStateIdx].count)
            {
                dinoRectSrc.x = dinoState[dinoStateIdx].offset + wDino * dinoActionIdx;
                dinoActionIdx++;
                if (dinoActionIdx == dinoState[dinoStateIdx].count)
                {
                    dinoActionIdx = 0;
                }
            }
            else
            {
                dinoActionIdx = 0;
            }
        }

        // dino 상태
        if (dinoRun == true && dinoWalk == true)
        {
            dinoStateIdx = 5;
            dinoSpeed = 10;
        }
        if (dinoWalk == true && dinoRun == false)
        {
            dinoStateIdx = 1;
            dinoSpeed = 5;
        }
        if (dinoJump == true && dinoWalk == true)
        {
            if (!dinoJumpLock)
            {
                dinoSpeedLock = dinoSpeed;
                dinoJumpLock = true;
            }
            dinoSpeed = dinoSpeedLock;
            dinoStateIdx = 4;

            if (dinoSpeed == 5)
            {
                dinoJumpHeight = 20;
            }
            else if (dinoSpeed == 10)
            {
                dinoJumpHeight = 25;
            }
            jump(dinoRectDes, dinoJumpHeight, hGround, jumpframe);
            jumpframe++;

            if (dinoRectDes.y > hGround)
            {
                dinoJump = false;
                dinoJumpLock = false;
                jumpframe = 0;
                dinoRectDes.y = hGround;
            }
        }


        // 배경 롤링
        if (bgRectSrc.x > window_w)
        {
            bgRectSrc.x = 0;
        }
        bgRectSrc.x += dinoSpeed;


        // Render
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, &bgRectSrc, &bgRectDes);
        SDL_RenderCopy(renderer, dinoTexture, &dinoRectSrc, &dinoRectDes);
        SDL_RenderPresent(renderer);


        // Control iteration time for Specific FPS
        frameTime = (Uint32)SDL_GetTicks64() - frameStart;
        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
        if (frameTime > 0)
        {
            //printf("FPS : %d\n", 1000/((Uint32)SDL_GetTicks64() - frameStart));
        }

        frameCount++;
    }

    // Close SDL
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

void jump(SDL_Rect& rect, int v0, int y0, int frame)
{
    rect.y = 0.5 * (frame * frame) - v0 * frame + y0;
}