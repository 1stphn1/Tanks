#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <random>
#include <cmath>
#include <ctime>

#include "Player.h"

constexpr int WINDOW_WIDTH  = 1280;
constexpr int WINDOW_HEIGHT = 720;

constexpr float RAD_IN_DEGREES = 57.2957795;
constexpr float PI             = 3.14159265;

constexpr int NUMBER_OF_PICTURES = 7;

#ifdef NDEBUG
    #define LOG(output) {}
#else
    #define LOG(output) std::cout << output << '\n'
#endif

#define PrintError() LOG(SDL_GetError())

class Game
{
public:
    static bool InitGame(const char *window_title);
    static void Clean();
    static void HandleEvents();
    static void Update();
    static void Render();
    static void RenderMap();
    static void BotRaiseOrLowerBarrel(bool raise);
    static void ReduceHP(bool to_human);
    static bool IsRunning();
    static SDL_Renderer *GetRenderer();

    static inline Player player;
    static inline Bot bot;

public:
    static inline int  terrain[WINDOW_WIDTH];  // Stores height for each terrain column 

    static inline bool running;
    static inline int  width;
    static inline int  height;
    static inline int  player_x, player_y;
    static inline int  bot_x, bot_y;

    static inline SDL_Window   *window;
    static inline SDL_Renderer *renderer;
    static inline SDL_Texture  *background_texture;
    static inline SDL_Texture  *ground_texture;
    static inline SDL_Texture  *surfacelay_texture;
    static inline SDL_Texture  *tank_texture;
    static inline SDL_Texture  *barrel_texture;
    static inline SDL_Texture  *projectile_texture;
}; // class Game

SDL_Texture *LoadTexture(const char *file_path);

namespace Noise
{
    float RandomFloat();   // Generate random float between 0 and 1
    float Noise(float x);  // Generate Perlin noise
    float Lerp(float a, float b, float t);  // Interpolate between two values
    void  GenerateTerrain();
}
