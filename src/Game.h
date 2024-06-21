#pragma once

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "Player.h"

constexpr int kWindowWidth  = 1280;
constexpr int kWindowHeight = 720;

constexpr float kRadInDegrees = 57.2957795;
constexpr float kPI           = 3.14159265;

constexpr int kNumberOfPictures = 6;

#ifdef NDEBUG
    #define LOG(output) {}
#else
    #define LOG(output) std::cout << (output) << '\n'
#endif

#define PRINT_ERROR() LOG(SDL_GetError())

class Game
{
public:
    static auto InitGame(const char *window_title) -> bool;
    static void Clean();
    static void HandleEvents();
    static void Update();
    static void Render();
    static void RenderMap();
    static void BotRaiseOrLowerBarrel(bool raise);
    static void ReduceHP(bool to_human);
    static void ProjectileGroundImpact(int impact_x);
    static auto IsRunning() -> bool;
    static auto GetRenderer() -> SDL_Renderer *;

    static inline Player sPlayer;
    static inline Bot sBot;

    static inline int  sTerrain[kWindowWidth];  // Stores height for each terrain column

    static inline bool sRunning;
    static inline int  sWidth;
    static inline int  sHeight;
    static inline int  sPlayerX, sPlayerY;
    static inline int  sBotX, sBotY;

    static inline SDL_Window   *sWindow;
    static inline SDL_Renderer *sRenderer;

    static inline SDL_Texture  *sBackgroundTexture;
    static inline SDL_Texture  *sGroundTexture;
    static inline SDL_Texture  *sSurfacelayTexture;
    static inline SDL_Texture  *sTankTexture;
    static inline SDL_Texture  *sBarrelTexture;
    static inline SDL_Texture  *sProjectileTexture;
}; // class Game

auto LoadTexture(const char *file_path) -> SDL_Texture *;

namespace Noise
{
    auto RandomFloat() -> float;   // Generate random float between 0 and 1
    auto Noise(float x_coord) -> float;  // Generate Perlin noise
    auto Lerp(float left, float right, float val) -> float;  // Interpolate between two values
    void GenerateTerrain();
}
