#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <vlc/vlc.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        std::cout << "SDL init error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "SEGA NEX",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cout << "Window error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cout << "Renderer error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 2 saniye siyah ekran
    Uint32 blackStart = SDL_GetTicks();
    SDL_Event e;
    bool quit = false;

    while (!quit && SDL_GetTicks() - blackStart < 2000) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    if (quit) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if (!SDL_GetWindowWMInfo(window, &wmInfo)) {
        std::cout << "SDL_GetWindowWMInfo error: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

#ifdef _WIN32
    void* hwnd = wmInfo.info.win.window;
#else
    std::cout << "This example is for Windows.\n";
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
#endif

    char* basePath = SDL_GetBasePath();
    if (!basePath) {
        std::cout << "SDL_GetBasePath error: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::string videoPath = std::string(basePath) + "assets/videos/startup.mp4";
    SDL_free(basePath);

    const char* vlcArgs[] = {
        "--no-video-title-show",
        "--quiet"
    };

    libvlc_instance_t* vlc = libvlc_new(2, vlcArgs);
    if (!vlc) {
        std::cout << "libvlc_new failed\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    libvlc_media_t* media = libvlc_media_new_path(vlc, videoPath.c_str());
    if (!media) {
        std::cout << "libvlc_media_new_path failed: " << videoPath << "\n";
        libvlc_release(vlc);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    libvlc_media_player_t* player = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    if (!player) {
        std::cout << "libvlc_media_player_new_from_media failed\n";
        libvlc_release(vlc);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    libvlc_media_player_set_hwnd(player, hwnd);
    libvlc_video_set_scale(player, 0.0f);

    if (libvlc_media_player_play(player) != 0) {
        std::cout << "libvlc_media_player_play failed\n";
        libvlc_media_player_release(player);
        libvlc_release(vlc);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool videoStarted = false;
    Uint32 startWait = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        libvlc_state_t state = libvlc_media_player_get_state(player);

        if (state == libvlc_Playing) {
            videoStarted = true;
        }

        if (videoStarted &&
            (state == libvlc_Ended || state == libvlc_Stopped || state == libvlc_Error)) {
            break;
        }

        if (!videoStarted && SDL_GetTicks() - startWait > 5000) {
            std::cout << "Video did not start in time: " << videoPath << "\n";
            break;
        }

        SDL_Delay(10);
    }

    libvlc_media_player_stop(player);
    libvlc_media_player_release(player);
    libvlc_release(vlc);

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}