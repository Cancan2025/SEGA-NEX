#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

int main(int argc, char* argv[])
{
    // Başlatma
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cout << "SDL init error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "IMG init error: " << IMG_GetError() << std::endl;
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Mixer init error: " << Mix_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SEGA NEX",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cout << "Window error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // BASE PATH
    char* basePath = SDL_GetBasePath();

    if (!basePath)
    {
        std::cout << "SDL_GetBasePath error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::string BASE_PATH = basePath;
    SDL_free(basePath);

    // Asset path helper (lambda captures BASE_PATH)
    auto GetAssetPath = [&](const std::string& file) {
        return BASE_PATH + "assets/" + file;
    };

    // Dosya yolları
    std::string phase1Path = GetAssetPath("images/startup_test.png");
    std::string phase2Path = GetAssetPath("images/startup.png");
    std::string soundPath = GetAssetPath("sounds/startup.wav");

    // Texture yükle
    SDL_Texture* phase1 = IMG_LoadTexture(renderer, phase1Path.c_str());
    SDL_Texture* phase2 = IMG_LoadTexture(renderer, phase2Path.c_str());

    if (!phase1)
    {
        std::cout << "Texture load error: " << IMG_GetError() << std::endl;
        std::cout << "Tried: " << phase1Path << std::endl;
        return 1;
    }

    if (!phase2)
    {
        std::cout << "Texture load error: " << IMG_GetError() << std::endl;
        std::cout << "Tried: " << phase2Path << std::endl;
        return 1;
    }

    // Ses yükle
    Mix_Chunk* bootSound = Mix_LoadWAV(soundPath.c_str());

    if (!bootSound)
    {
        std::cout << "Sound load error: " << Mix_GetError() << std::endl;
        std::cout << "Tried: " << soundPath << std::endl;
        return 1;
    }

    Uint32 startTime = SDL_GetTicks();
    bool quit = false;
    bool soundPlayed = false;
    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        Uint32 elapsed = SDL_GetTicks() - startTime;

        // Temizle
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 0–2 saniye arası siyah ekran
        if (elapsed < 2000) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }
        // 2–4 saniye arası beyaz ekran
        else if (elapsed < 4000) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
        }
        // 4 saniyede sesi başlat
        if (elapsed >= 4000 && !soundPlayed) {
            Mix_PlayChannel(-1, bootSound, 0);
            soundPlayed = true;
        }

        if (elapsed >= 4000)
        {
            float progress = (elapsed - 4000) / 500.0f;
            if (progress > 1.0f) progress = 1.0f;

            int alpha = (int)(progress * 255);

            SDL_SetTextureAlphaMod(phase1, alpha);
            SDL_RenderCopy(renderer, phase1, NULL, NULL);
        }

        // 5 saniyeden sonra → phase2 direkt üstüne göster
        if (elapsed >= 5000)
        {
            SDL_RenderCopy(renderer, phase2, NULL, NULL);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Temizlik
    SDL_DestroyTexture(phase1);
    SDL_DestroyTexture(phase2);
    Mix_FreeChunk(bootSound);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
    return 0;
}