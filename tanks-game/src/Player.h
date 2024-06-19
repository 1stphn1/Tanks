#pragma once

#include "Game.h"

#define POS_X   dest_rect.x
#define POS_Y   dest_rect.y

#define TANK_WIDTH       128  // in pixels
#define TANK_HEIGHT       64

#define BARREL_WIDTH      50  // in pixels
#define BARREL_HEIGHT     8

#define BARREL_X          80  // in pixels
#define BARREL_Y          35

#define PROJECTILE_WIDTH  32
#define PROJECTILE_HEIGHT 32

#define MAX_HP           100

#define IDEAL_DISTANCE  -500

#define PROJECTILE_VELOCITY  13  // pixels per frame
#define GRAVITY_ACC         0.1  // pixels per frame^2

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
    double x_velocity = 0;
    double y_velocity = 0;
    double x;
    double y;
    
    bool is_present = false;

    SDL_Rect dest_rect = {0, 0, 0, 0};
    Timer timer;

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
    int hp;
    bool is_human;

    long double tank_angle;
    long double gun_barrel_angle;  // Relative to the tank

    Projectile bullet;

    SDL_Rect dest_rect;
    SDL_Rect barrel_dest_rect;
};

class Bot : public Player
{
public:
    Bot();

    void Action(int player_x);
};