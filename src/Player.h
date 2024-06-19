#pragma once

#include <SDL2/SDL.h>
#include <time.h>

constexpr int TANK_WIDTH  = 128;  // in pixels
constexpr int TANK_HEIGHT = 64;

constexpr int BARREL_WIDTH  = 50;  // in pixels
constexpr int BARREL_HEIGHT = 8;

constexpr int BARREL_X = 80;  // in pixels
constexpr int BARREL_Y = 35;

constexpr int PROJECTILE_WIDTH  = 32;
constexpr int PROJECTILE_HEIGHT = 32;

constexpr int MAX_HP = 100;

constexpr int IDEAL_DISTANCE = -500;

constexpr int   PROJECTILE_VELOCITY = 13;  // pixels per frame
constexpr float GRAVITY_ACC         = 0.1f;  // pixels per frame^2

struct Timer
{
public:
    Timer();
    
    void    StartTiming();
    clock_t GetTime();

private:
    clock_t start_time;
};

struct Projectile 
{
    double mXVelocity = 0;
    double mYVelocity = 0;
    double mX;
    double mY;
    
    bool mIsPresent = false;

    SDL_Rect mRenderRect = {0, 0, 0, 0};
    Timer mTimer;

    void Fire(double angle, double barrel_angle, const SDL_Rect& barrel_dest_rect);
    void Move(bool is_human);
};

class Player
{
public:
    Player();
    ~Player();

    void Update();
    void Render();
    void Fire();
    void ReduceHP();
    void IncrementX(int val_to_add);
    void DecrementX(int val_to_take);
    void IncrementGunBarrelAngle();
    void DecrementGunBarrelAngle();

    int GetX();
    int GetY();
    int GetHP();
    double GetBarrelAngle();
    SDL_Rect GetDestRect();

protected:
    int mHP;
    bool mIsHuman;

    double mTankAngle;
    double mGunBarrelAngle;  // Relative to the tank

    Projectile mProjectile;

    SDL_Rect mRenderRect;
    SDL_Rect mBarrelRenderRect;
};

class Bot : public Player
{
public:
    Bot();

    void Action(int player_x);
};
