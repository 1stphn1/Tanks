#pragma once

#include <SDL2/SDL.h>
#include <ctime>

constexpr int kTankWidth  = 128;  // in pixels
constexpr int kTankHeight = 64;

constexpr int kBarrelWidth  = 50;  // in pixels
constexpr int kBarrelHeight = 8;

constexpr int kBarrelX = 80;  // in pixels
constexpr int kBarrelY = 35;

constexpr int kProjectileWidth  = 32;
constexpr int kProjectileHeight = 32;

constexpr int kMaxHp = 100;

constexpr int kIdealDistance = -500;

constexpr int   kProjectileVelocity = 13;  // pixels per frame
constexpr float kGravityAcc         = 0.1F;  // pixels per frame^2

struct Timer
{
public:
    Timer();
    
    void StartTiming();
    [[nodiscard]] auto GetTime() const -> clock_t;

private:
    clock_t mStartTime;
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

    void Fire(double angle, double barrel_angle, const SDL_Rect &barrel_dest_rect);
    void Move(bool is_human);
};

class Player
{
public:
    Player();
    ~Player() = default;

    void Update();
    void Render();
    void Fire();
    void ReduceHP();
    void IncrementX(int val_to_add);
    void DecrementX(int val_to_take);
    void IncrementGunBarrelAngle();
    void DecrementGunBarrelAngle();

    [[nodiscard]] auto GetX() const -> int;
    [[nodiscard]] auto GetY() const -> int;
    [[nodiscard]] auto GetHP() const -> int;
    [[nodiscard]] auto GetBarrelAngle() const -> double;
    [[nodiscard]] auto GetDestRect() const -> SDL_Rect;

protected:
    int mHP;
    bool mIsHuman;

    double mTankAngle;
    double mGunBarrelAngle;  // Relative to the tank

    SDL_Rect mRenderRect;
    SDL_Rect mBarrelRenderRect;

    Projectile mProjectile;
};

class Bot : public Player
{
public:
    Bot();
    void Action(int player_x);
};
