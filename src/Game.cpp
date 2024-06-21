#include "Game.h"

#include <iostream>
#include <random>
#include <cmath>
#include <ctime>

const char *background_pictures[kNumberOfPictures] = {
    "assets/background1.png",
    "assets/background2.jpg",
    "assets/background3.jpg",
    "assets/background5.jpg",
    "assets/background6.jpg",
    "assets/background7.jpg"
};

auto Game::InitGame(const char *window_title) -> bool
{
    sWidth  = kWindowWidth;  // TODO(1stphn1): make it get sWidth and sHeight from system resolution
    sHeight = kWindowHeight;

    sRunning = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LOG("error at SDL_Init");
        PRINT_ERROR();
        return false;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        LOG("error at IMG_Init");
        PRINT_ERROR();
        return false;
    }

    sWindow = SDL_CreateWindow(window_title, 200, 200, sWidth, sHeight, SDL_WINDOW_SHOWN);

    if (sWindow == nullptr) {
        PRINT_ERROR();
        return false;
    }

    sRenderer = SDL_CreateRenderer(sWindow, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (sRenderer == nullptr) {
        PRINT_ERROR();
        return false;
    }

    SDL_SetRenderDrawColor(sRenderer, 0xe0, 0xf, 0xf, 0xff);  // Sets to red for rendering health bars

    srand(time(nullptr));

    sBackgroundTexture = LoadTexture(background_pictures[rand() % kNumberOfPictures]);
    sGroundTexture     = LoadTexture("assets/ground.jpg");
    sSurfacelayTexture = LoadTexture("assets/surface.png");
    sTankTexture       = LoadTexture("assets/tank.png");
    sBarrelTexture     = LoadTexture("assets/barrel.png");
    sProjectileTexture = LoadTexture("assets/projectile.png");

    Noise::GenerateTerrain();

    for (int i = 0; i < kWindowWidth - 1; i++) {  // Smoothens the sTerrain a little bit
        if (Game::sTerrain[i + 1] > Game::sTerrain[i]) {
            Game::sTerrain[i + 1] = Game::sTerrain[i] + 1;
        } else if (Game::sTerrain[i + 1] < Game::sTerrain[i]) {
            Game::sTerrain[i + 1] = Game::sTerrain[i] - 1;
	   }
    }

    /* for (int i = 1; i < kWindowWidth - 1; i++) {  // Smoothens the sTerrain, but different pairs */
    /*     if (Game::sTerrain[i + 1] > Game::sTerrain[i]) { */
    /*         Game::sTerrain[i + 1] = Game::sTerrain[i] + 1; */
    /*     } else if (Game::sTerrain[i + 1] < Game::sTerrain[i]) { */
    /*         Game::sTerrain[i + 1] = Game::sTerrain[i] - 1; */
	   /* } */
    /* } */

    sPlayer = Player();
    sBot = Bot();

    sRunning = true;
    return true;
}

void Game::Clean()
{
    sRunning = false;

    SDL_DestroyTexture(sBackgroundTexture);
    SDL_DestroyTexture(sSurfacelayTexture);
    SDL_DestroyTexture(sProjectileTexture);
    SDL_DestroyTexture(sGroundTexture);
    SDL_DestroyTexture(sBarrelTexture);
    SDL_DestroyTexture(sTankTexture);

    SDL_DestroyWindow(sWindow);
    SDL_DestroyRenderer(sRenderer);
    SDL_Quit();
    IMG_Quit();
}

void Game::HandleEvents()
{
    SDL_Event event;
    sBot.Action(sPlayer.GetX());

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            sRunning = false;
            return;
        }

        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                if (sPlayer.GetX() >= 3) {
                    sPlayer.DecrementX(3);
			 }
                break;
            case SDLK_RIGHT:
                if (sPlayer.GetX() + kTankWidth <= kWindowWidth - 3) {
                    sPlayer.IncrementX(3);
			 }
                break;
            case SDLK_UP:
                if (sPlayer.GetBarrelAngle() > -65) {
                   sPlayer.DecrementGunBarrelAngle();
			 }
                break;
            case SDLK_DOWN:
                if (sPlayer.GetBarrelAngle() < 5) {
                    sPlayer.IncrementGunBarrelAngle();
			 }
                break;
            case SDLK_SPACE:
                sPlayer.Fire();
                break;
            case SDLK_ESCAPE:
                Clean();
                break;
		  default:
			 break;
        }
    }
}

void Game::Update()
{
    if (sPlayer.GetHP() <= 0) {
        LOG("You lost");
        sRunning = false;
        return;
    }

    if (sBot.GetHP() <= 0) {
        LOG("You won");
        sRunning = false;
        return;
    }

    sPlayerX = sPlayer.GetX();
    sPlayerY = sPlayer.GetY();
    sBotX = sBot.GetX();
    sBotY = sBot.GetY();

    sPlayer.Update();  
    sBot.Update(); 
}

void Game::Render()
{
    SDL_RenderClear(sRenderer);

    RenderMap();
    sPlayer.Render();
    sBot.Render();

    SDL_RenderPresent(sRenderer);
}

auto Game::IsRunning() -> bool
{
    return sRunning;
}

auto Game::GetRenderer() -> SDL_Renderer*
{
    return sRenderer;
}

void Game::RenderMap()
{
    SDL_RenderCopy(sRenderer, sBackgroundTexture, nullptr, nullptr);

    const int ground_image_width  = 1600;
    const int ground_image_height = 800;

    SDL_Rect src_rect;
    SDL_Rect dst_rect;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = 1;
    src_rect.h = ground_image_height;

    dst_rect.x = 0;
    dst_rect.w = 1;
    dst_rect.h = ground_image_height;

    for (int height : sTerrain) {  // This code renders sTerrain pixel by pixel. That's why src_rect.x increases by 1 every iteration        
        dst_rect.y = kWindowHeight - height;  // Calculates y from sTerrain sHeight
        src_rect.y = ground_image_height - height;  // Calculates src_rect.y so the texture would't stretch
        dst_rect.h = height;

        SDL_RenderCopy(sRenderer, sGroundTexture, &src_rect, &dst_rect);

        dst_rect.x++;
	   src_rect.x++;

        if (src_rect.x == ground_image_width) {   // Starts rendering texture from begging if the end of the texture is reached
            src_rect.x = 0;
        }
    }

    SDL_Rect dst_rect_surface_layer;

    dst_rect_surface_layer.x = 0;
    dst_rect_surface_layer.y = 0;
    dst_rect_surface_layer.w = 20;
    dst_rect_surface_layer.h = 20;

    for (int i = 0; i < kWindowWidth; i++) {  // Renders surface layer
        dst_rect_surface_layer.y = kWindowHeight - sTerrain[i];

        if (i != 0 && 
		  (kWindowHeight - sTerrain[i - 1]) - (kWindowHeight - sTerrain[i]) <= 5  &&
		  (kWindowHeight - sTerrain[i - 1]) - (kWindowHeight - sTerrain[i]) >= -5)
	   {
            dst_rect_surface_layer.y = kWindowHeight - sTerrain[i - 1];
        }

        SDL_RenderCopy(sRenderer, sSurfacelayTexture, nullptr, &dst_rect_surface_layer);
        dst_rect_surface_layer.x++;
    }
}

void Game::BotRaiseOrLowerBarrel(bool raise)
{
    if (raise && sBot.GetBarrelAngle() < 5) {
        sBot.IncrementGunBarrelAngle();
    } else if (sBot.GetBarrelAngle() > -65) {
        sBot.DecrementGunBarrelAngle();
    }
}

void Game::ReduceHP(bool to_human)
{
    if (to_human) {
        sPlayer.ReduceHP();
    } else {
        sBot.ReduceHP();
    }
}

void Game::ProjectileGroundImpact(int impact_x)
{
    sTerrain[impact_x] -= static_cast<int>(std::sin(45 / kRadInDegrees) * 30.0);

    for (int i = 1; i < 45; i++) {  // Reduces sHeight of the sTerrain circulary
	   sTerrain[impact_x + (45 - i)] -= static_cast<int>(std::sin(static_cast<double>(i) / kRadInDegrees) * 30.0);
	   sTerrain[impact_x - (45 - i)] -= static_cast<int>(std::sin(static_cast<double>(i) / kRadInDegrees) * 30.0);
    }

    /* for (int i = 0; i < kWindowWidth - 1; i++) {  // Smoothes the sTerrain a little bit */
	   /* if (sTerrain[i + 1] > sTerrain[i]) { */
		  /* sTerrain[i + 1] = sTerrain[i] + 1; */
	   /* } else if (sTerrain[i + 1] < sTerrain[i]) { */
		  /* sTerrain[i + 1] = sTerrain[i] - 1; */
	   /* } */
    /* } */
}

auto LoadTexture(const char *file_path) -> SDL_Texture*  // Returns null on failure
{
    SDL_Texture *texture = IMG_LoadTexture(Game::sRenderer, file_path);

    if (texture == nullptr) {
	   LOG("texture is null in LoadTexture");
    }

    return texture;
}

auto Distance(int x_1, int y_1, int x_2, int y_2) -> double
{
    return std::sqrt( std::pow(x_1 - x_2, 2) + std::pow(y_1 - y_2, 2) );
}

namespace Noise
{
    auto RandomFloat() -> float 
    {
        static std::default_random_engine engine(std::random_device{}()); // Seed with a random device
        static std::uniform_real_distribution<float> dist(0.0F, 1.0F);
        return dist(engine);
    }

    auto Noise(float x_coord) -> float
    {
        int x_0 = static_cast<int>(std::floor(x_coord));
        float lerp_val = x_coord - static_cast<float>(x_0);
        float v_0 = RandomFloat();
        float v_1 = RandomFloat();
        return (1 - lerp_val) * v_0 + lerp_val * v_1;
    }

    auto Lerp(float left, float right, float val) -> float 
    {
        return (1 - val) * left + val * right;
    }

    void GenerateTerrain()
    {
        constexpr float kRange = 0.5F;
        constexpr float kFrequency = 0.05F;
        constexpr float kAmplitude = 1.0F;
        constexpr float kPersistence = 0.5F;

        for (int i = 0; i < kWindowWidth; ++i) {
            float total = 0.0F;
            float freq = kFrequency;
            float amp = kAmplitude;

            for (int j = 0; j < 4; ++j) {
                total += Noise(static_cast<float>(i) * freq) * amp;
                freq *= 2.0F;
                amp *= kPersistence;
            }

		  Game::sTerrain[i] = static_cast<int>(std::round(total * (kWindowHeight / 2.0F) * kRange));
        }
    }
}
