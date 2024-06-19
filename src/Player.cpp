#include "Player.h"
#include "Game.h"

Timer::Timer()
{
    StartTiming();
}

void Timer::StartTiming()
{
    start_time = clock();
}

clock_t Timer::GetTime()
{
    return clock() - start_time;
}

Player::Player()
{
    mTankAngle = 0;
    mGunBarrelAngle = 0;

    mRenderRect.w = TANK_WIDTH;
    mRenderRect.h = TANK_HEIGHT;

    mRenderRect.x = WINDOW_WIDTH / 4;

    mBarrelRenderRect.w = BARREL_WIDTH;
    mBarrelRenderRect.h = BARREL_HEIGHT;

    mIsHuman = true;
    mHP = MAX_HP;
}

Player::~Player()
{

}

Bot::Bot()
{
    mRenderRect.x = 3 * WINDOW_WIDTH / 4;

    mBarrelRenderRect.w = BARREL_WIDTH;
    mBarrelRenderRect.h = BARREL_HEIGHT;

    mIsHuman = false;
}

void Player::Update()
{
    mRenderRect.y = Game::height - Game::terrain[mRenderRect.x] - TANK_HEIGHT;  // Calculates y position from terrain height at tank's x position

    int x2 = mRenderRect.x + std::cos(mTankAngle / RAD_IN_DEGREES) * (double)TANK_WIDTH;  // Calculates 2nd x position (the other end of the tank) by tank angle
    int y2 = Game::height - Game::terrain[x2] - TANK_HEIGHT;  // Calculates 2nd y position based on height at x2

    mTankAngle = -std::asin(static_cast<double>(mRenderRect.y - y2) / TANK_WIDTH) * RAD_IN_DEGREES;  // Calculates tank angle using asin of sin(angle)
                                                                                                // sin(angle) = opposite / hypothenuse

    // Calculates positions using circle coordinate formulas for x and y. Calculations are not fully precise for some reason. They are more precise with BARREL_X + 7
    mBarrelRenderRect.x = mRenderRect.x + (double)(BARREL_X) * std::cos(std::abs(mTankAngle) / RAD_IN_DEGREES);
    mBarrelRenderRect.y = mRenderRect.y + BARREL_Y + (double)(BARREL_X) * std::sin(mTankAngle / RAD_IN_DEGREES);

    if (!mIsHuman)  // Bot is flipped horizontally, so barrel needs to be moved
	   mBarrelRenderRect.x -= BARREL_X - 3;

    if (mProjectile.mIsPresent)
        mProjectile.Move(mIsHuman);
}

void Player::Render()
{
    SDL_Point center;

    center.x = (mIsHuman? 0 : BARREL_WIDTH);
    center.y = BARREL_HEIGHT;

    SDL_RenderCopyEx(
	   Game::renderer,
	   Game::barrel_texture,
	   nullptr,
	   &mBarrelRenderRect,
	   mGunBarrelAngle + mTankAngle,
	   &center,
	   mIsHuman? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    );

    center.x = 0;
    center.y = BARREL_Y;
    SDL_RenderCopyEx(
	   Game::renderer,
	   Game::tank_texture,
	   nullptr,
	   &mRenderRect,
	   mTankAngle,
	   &center,
	   mIsHuman? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    );

    if (mProjectile.mIsPresent) {
        SDL_RenderCopyEx(
		  Game::renderer,
		  Game::projectile_texture,
		  nullptr,
		  &mProjectile.mRenderRect,
		  0,
		  nullptr,
		  mIsHuman? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
	   );
    }

    int hpbar_x;

    if (mIsHuman)
        hpbar_x = 50;
    else
        hpbar_x = WINDOW_WIDTH - 250;

    for (int i = 0; i < mHP * 2; i += 2) {
        SDL_RenderDrawLine(Game::renderer, hpbar_x + i, 20, hpbar_x + i, 50);
        SDL_RenderDrawLine(Game::renderer, hpbar_x + i + 1, 20, hpbar_x + i + 1, 50);
    }
}

void Player::Fire()
{
    mProjectile.Fire(mGunBarrelAngle + mTankAngle, mGunBarrelAngle, mBarrelRenderRect);
}

void Player::ReduceHP()
{
    mHP -= 5;
}

SDL_Rect Player::GetDestRect()
{
    return mRenderRect;
}

void Player::IncrementX(int val_to_add)
{
    mRenderRect.x += val_to_add;
    mBarrelRenderRect.x += val_to_add;
}

void Player::DecrementX(int val_to_take)
{
    mRenderRect.x -= val_to_take;
    mBarrelRenderRect.x -= val_to_take;
}

void Player::IncrementGunBarrelAngle()
{
    mGunBarrelAngle++;
}

void Player::DecrementGunBarrelAngle()
{
    mGunBarrelAngle--;
}

int Player::GetX()
{
    return mRenderRect.x;
}

int Player::GetY()
{
    return mRenderRect.y;
}

int Player::GetHP()
{
    return mHP;
}

double Player::GetBarrelAngle()
{
    return mGunBarrelAngle;
}

void Projectile::Fire(double angle, double barrel_angle, const SDL_Rect& barrel_dest_rect)
{
    if (mTimer.GetTime() < 2000 || this->mIsPresent)
        return;

    angle *= -1;  // SDL_RenderCopyEx function rotates up if the angle is negative. Here it needs to be positive
    mYVelocity = (angle / 90.0) * PROJECTILE_VELOCITY;  // If the angle is 90, then all the velocity is on the y axis

    mXVelocity = PROJECTILE_VELOCITY - mYVelocity; // Tank always fires with the same momentum, so when fired x speed + y speed should always be the same, regardless of the angle

    barrel_angle *= -1;

    mRenderRect.w = 32;
    mRenderRect.h = 32;
    mRenderRect.x = mX = barrel_dest_rect.x + BARREL_WIDTH + 20 - std::cos(barrel_angle / RAD_IN_DEGREES) * (double)BARREL_WIDTH;  // Calculates the bullet position
    mRenderRect.y = mY = barrel_dest_rect.y - BARREL_HEIGHT - 10 - std::sin(barrel_angle / RAD_IN_DEGREES) * (double)BARREL_WIDTH;

    mIsPresent = true;
    mTimer.StartTiming();
}

void Projectile::Move(bool is_human)
{
    if (mY <= PROJECTILE_HEIGHT || mY >= WINDOW_HEIGHT - PROJECTILE_HEIGHT || mX <= PROJECTILE_WIDTH || mX >= WINDOW_WIDTH - PROJECTILE_WIDTH) {  // If the bullet is out of screen bounds
        mIsPresent = false;
        return;
    }

    mYVelocity -= GRAVITY_ACC;  // Decres y velocity by g

    if (is_human)  // If the human fired the projectile, it goes to right. Bot's projectiles go to left
        mX += mXVelocity;
    else
        mX -= mXVelocity;

    mY -= mYVelocity;  // Decreases y coordinate by mYVelocity

    mRenderRect.x = mX;
    mRenderRect.y = mY;

    if (Game::height - mRenderRect.y <= Game::terrain[mRenderRect.x]) {  // If projectile hit the ground
        mIsPresent = false;

        Game::terrain[mRenderRect.x] -= std::sin(45 / RAD_IN_DEGREES) * 30;

        for (int i = 1; i < 45; i++) {  // Reduces height of the terrain circulary
            Game::terrain[mRenderRect.x + (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
            Game::terrain[mRenderRect.x - (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
        }

        for (int i = 0; i < WINDOW_WIDTH - 1; i++) {  // Smoothes the terrain a little bit
            if (Game::terrain[i + 1] > Game::terrain[i])
                Game::terrain[i + 1] = Game::terrain[i] + 1;
            else if (Game::terrain[i + 1] < Game::terrain[i])
                Game::terrain[i + 1] = Game::terrain[i] - 1;
        }

        if (!is_human)  // Determines wether bot should raise or lower the barrel by knowing if projectile hit the ground behind or in frot of player
            Game::BotRaiseOrLowerBarrel(mRenderRect.x > Game::player_x? true : false);

        return;
    }

    int enemy_x, enemy_y;

    if (is_human) {  // If the player is human, the enemy is bot, and vice versa
        enemy_x = Game::bot_x;
        enemy_y = Game::bot_y;
    } else {
        enemy_x = Game::player_x;
        enemy_y = Game::player_y;
    }

    if (mRenderRect.x > enemy_x && mRenderRect.x < enemy_x + TANK_WIDTH && mRenderRect.y > enemy_y && mRenderRect.y < enemy_y + TANK_HEIGHT) {  // Checks wether the projectile hit the enemy
        mIsPresent = false;

        Game::ReduceHP(!is_human);

        Game::terrain[mRenderRect.x] -= std::sin(45 / RAD_IN_DEGREES) * 30;  // This code is the same as the one above

        for (int i = 1; i < 45; i++) {
            Game::terrain[mRenderRect.x + (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
            Game::terrain[mRenderRect.x - (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
        }

        for (int i = 0; i < WINDOW_WIDTH - 1; i++) {
            if (Game::terrain[i + 1] > Game::terrain[i])
                Game::terrain[i + 1] = Game::terrain[i] + 1;
            else if (Game::terrain[i + 1] < Game::terrain[i])
                Game::terrain[i + 1] = Game::terrain[i] - 1;
        }
    }
}

void Bot::Action(int player_x)  // Determines what should bot do
{
    /* const int FIRE_PAUSE = 2000; */

    /* static clock_t start_time = clock(); */
    /* static clock_t end_time; */

    /* end_time = clock(); */

    if (player_x - mRenderRect.x > IDEAL_DISTANCE && mRenderRect.x + TANK_WIDTH < WINDOW_WIDTH)
        mRenderRect.x++;
    else if (player_x - mRenderRect.x < IDEAL_DISTANCE && mRenderRect.x > 0)
        mRenderRect.x--;

    if (player_x - mRenderRect.x > IDEAL_DISTANCE - 50 || player_x - mRenderRect.x < IDEAL_DISTANCE + 50) {
        this->Fire();

        /* start_time = clock(); */
    }
}
