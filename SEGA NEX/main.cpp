#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <vlc/vlc.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct VideoContext {
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> pixels;
    std::mutex mutex;
    int videoWidth, videoHeight;
};

static void* lock(void* data, void** p_pixels) {
    VideoContext* ctx = (VideoContext*)data;
    ctx->mutex.lock();
    *p_pixels = ctx->pixels.data();
    return NULL;
}

static void unlock(void* data, void* id, void* const* p_pixels) {
    VideoContext* ctx = (VideoContext*)data;
    ctx->mutex.unlock();
}

int main(int argc, char* argv[]) {
    // --- 1. SDL Temel Kurulumu ---
    // Path (yol) alabilmek için SDL'i en başta başlatıyoruz.
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        return -1;
    }

    // Uygulamanın çalıştığı ana dizini alıyoruz
    char* basePath = SDL_GetBasePath();
    std::string baseDir = basePath ? std::string(basePath) : "./";

    // --- 2. Dinamik Config Okuma ---
    std::string configPath = baseDir + "config/config.json";
    std::string screenType = "16:9 (720p - 1280x720)";
    bool isFullscreen = false;

    std::ifstream configFile(configPath);
    if (configFile.is_open()) {
        try {
            json config;
            configFile >> config;
            if (config.contains("screenType")) screenType = config["screenType"].get<std::string>();
            if (config.contains("fullscreen")) isFullscreen = config["fullscreen"].get<bool>();
        }
        catch (...) {
            std::cerr << "Config dosyasi okunurken hata olustu, varsayilanlar kullaniliyor." << std::endl;
        }
        configFile.close();
    }

    // Ayarlara göre değişkenleri belirle
    int baseW = 1280, baseH = 720;
    bool isInterlaced = false;
    std::string videoFilename = "startup.mp4";

    if (screenType == "4:3 (480i - 640x480 interlaced)") {
        baseW = 640; baseH = 480;
        isInterlaced = true;
        videoFilename = "startup_sd.mp4";
    }

    // --- 3. Pencere ve Renderer Oluşturma ---
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (isFullscreen) windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    SDL_Window* window = SDL_CreateWindow("SEGA NEX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, baseW, baseH, windowFlags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    VideoContext ctx;
    ctx.renderer = renderer;
    ctx.videoWidth = baseW;
    ctx.videoHeight = baseH;
    ctx.pixels.resize(baseW * baseH);
    ctx.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, baseW, baseH);

    // --- 4. VLC Kurulumu ---
    std::string videoPath = baseDir + "assets/videos/" + videoFilename;

    // basePath ile işimiz bitti, belleği boşaltıyoruz
    if (basePath) SDL_free(basePath);

    const char* vlcArgs[] = { "--no-video-title-show", "--quiet" };
    libvlc_instance_t* vlc = libvlc_new(2, vlcArgs);
    libvlc_media_t* media = libvlc_media_new_path(vlc, videoPath.c_str());
    libvlc_media_player_t* mp = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    libvlc_video_set_callbacks(mp, lock, unlock, NULL, &ctx);
    libvlc_video_set_format(mp, "RV32", baseW, baseH, baseW * 4);

    // --- 5. Zamanlama ve Durum Değişkenleri ---
    bool videoBittiMi = false;
    bool vlcDurduruldu = false;
    bool videoBasladiMi = false;
    Uint32 baslangicZamani = SDL_GetTicks();

    bool quit = false;
    SDL_Event ev;

    // --- 6. Ana Render Döngüsü ---
    while (!quit) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
        }

        // İlk 2 saniye bekleme mantığı
        if (!videoBasladiMi && SDL_GetTicks() - baslangicZamani >= 2000) {
            libvlc_media_player_play(mp);
            videoBasladiMi = true;
        }

        int renderW, renderH;
        SDL_GetRendererOutputSize(renderer, &renderW, &renderH);

        // Ekranı temizle (Siyah zemin)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        libvlc_state_t state = libvlc_media_player_get_state(mp);

        // Video çizimi
        if (videoBasladiMi && !videoBittiMi && state != libvlc_Ended && state != libvlc_Stopped && state != libvlc_Error) {

            float screenAspect = (float)renderW / renderH;
            float videoAspect = (float)baseW / baseH;
            SDL_Rect destRect;

            if (screenAspect > videoAspect) {
                destRect.h = renderH;
                destRect.w = (int)(renderH * videoAspect);
                destRect.x = (renderW - destRect.w) / 2;
                destRect.y = 0;
            }
            else {
                destRect.w = renderW;
                destRect.h = (int)(renderW / videoAspect);
                destRect.x = 0;
                destRect.y = (renderH - destRect.h) / 2;
            }

            ctx.mutex.lock();
            SDL_UpdateTexture(ctx.texture, NULL, ctx.pixels.data(), baseW * 4);
            ctx.mutex.unlock();
            SDL_RenderCopy(renderer, ctx.texture, NULL, &destRect);
        }

        // Interlaced (Tarama Çizgisi) Efekti
        if (isInterlaced) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
            for (int y = 0; y < renderH; y += 2) {
                SDL_RenderDrawLine(renderer, 0, y, renderW, y);
            }
        }

        SDL_RenderPresent(renderer);

        // VLC Güvenli Durdurma
        if (state == libvlc_Ended && !vlcDurduruldu) {
            videoBittiMi = true;
            libvlc_media_player_stop(mp);
            vlcDurduruldu = true;
        }

        SDL_Delay(10);
    }

    // --- 7. Kapanış ve Temizlik ---
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(vlc);
    SDL_DestroyTexture(ctx.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}