#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <random>
#include <cmath>
#include <ctime>

#include "Player.h"

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

#define RAD_IN_DEGREES  57.2957795
#define PI              3.14159265

#define LOG(output)   std::cout << output << '\n'
#define PrintError()  LOG(SDL_GetError())

#define NUMBER_OF_PICTURES  7

namespace Game
{
    bool InitGame(const char *window_title);
    void Clean();

    void HandleEvents();
    void Update();
    void Render();
    void RenderMap();
    void BotRaiseOrLowerBarrel(bool raise);
    void ReduceHP(bool to_human);

    bool IsRunning();

    SDL_Renderer *GetRenderer();

    extern int  terrain[WINDOW_WIDTH];  // Stores height for each terrain column 

    extern bool running;
    extern int  width;
    extern int  height;
    extern int  player_x, player_y;
    extern int  bot_x, bot_y;

    extern SDL_Window   *window;
    extern SDL_Renderer *renderer;
    extern SDL_Texture  *background_texture;
    extern SDL_Texture  *ground_texture;
    extern SDL_Texture  *surfacelay_texture;
    extern SDL_Texture  *tank_texture;
    extern SDL_Texture  *barrel_texture;
    extern SDL_Texture  *projectile_texture;
} // namespace Game

SDL_Texture *LoadTexture(const char *file_path);

float randomFloat();   // Generate random float between 0 and 1
float noise(float x);  // Generate Perlin noise
float lerp(float a, float b, float t);  // Interpolate between two values
void  generateTerrain();