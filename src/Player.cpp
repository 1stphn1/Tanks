#include "Player.h"
#include "Game.h"

#include <iostream>

Timer::Timer()
{
    StartTiming();
}

void Timer::StartTiming()
{
    mStartTime = clock();
}

auto Timer::GetTime() const -> clock_t
{
    return clock() - mStartTime;
}

Player::Player() :
    mHP(kMaxHp),
    mIsHuman(true),
    mTankAngle(0),
    mGunBarrelAngle(0),
    mRenderRect({ 0, 0, kTankWidth, kTankHeight }),
    mBarrelRenderRect({ 0, 0, kBarrelWidth, kBarrelHeight }),
    mProjectile()
{
}

Bot::Bot()
{
    mRenderRect.x = 3 * kWindowWidth / 4;

    mBarrelRenderRect.w = kBarrelWidth;
    mBarrelRenderRect.h = kBarrelHeight;

    mIsHuman = false;
}

void Player::Update()
{
    static int old_human_tank_x = Game::sPlayerX;
    static int old_bot_tank_x = Game::sBotX;

    if ((mIsHuman && old_human_tank_x != Game::sPlayerX) || (!mIsHuman && old_bot_tank_x != Game::sBotX)) {
	   mRenderRect.y = Game::sHeight - Game::sTerrain[mRenderRect.x] - kTankHeight;  // Calculates y position from sTerrain sHeight at tank's x position
	   int tank_right_end_x = mRenderRect.x + static_cast<int>(std::cos(mTankAngle / kRadInDegrees)) * kTankWidth;  // Calculates 2nd x position (other end of the tank) by tank angle
	   int tank_right_end_y = Game::sHeight - Game::sTerrain[tank_right_end_x] - kTankHeight;  // Calculates 2nd y position based on sHeight at x2
	   mTankAngle = -std::asin(static_cast<double>(mRenderRect.y - tank_right_end_y) / kTankWidth) * kRadInDegrees;  // Calculates tank angle using asin of sin(angle)
                                                                                                // sin(angle) = opposite / hypothenuse
	   if (mIsHuman) {
		  old_human_tank_x = Game::sPlayerX;
	   } else {
		  old_bot_tank_x = Game::sBotX;
	   }
    }

    // Calculates positions using circle coordinate formulas for x and y. Calculations are not fully precise for some reason. They are more precise with kBarrelX + 7
    mBarrelRenderRect.x = mRenderRect.x + static_cast<int>(kBarrelX * std::cos(std::abs(mTankAngle) / kRadInDegrees));
    mBarrelRenderRect.y = mRenderRect.y + kBarrelY + static_cast<int>(kBarrelX * std::sin(mTankAngle / kRadInDegrees));

    if (!mIsHuman) {  // Bot is flipped horizontally, so barrel needs to be moved
	   mBarrelRenderRect.x -= kBarrelX - 3;
    }

    if (mProjectile.mIsPresent) {
        mProjectile.Move(mIsHuman);
    }
}

void Player::Render()
{
    SDL_Point center;

    center.x = (mIsHuman? 0 : kBarrelWidth);
    center.y = kBarrelHeight;

    SDL_RenderCopyEx(
	   Game::sRenderer,
	   Game::sBarrelTexture,
	   nullptr,
	   &mBarrelRenderRect,
	   mGunBarrelAngle + mTankAngle,
	   &center,
	   mIsHuman? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    );

    center.x = 0;
    center.y = kBarrelY;
    SDL_RenderCopyEx(
	   Game::sRenderer,
	   Game::sTankTexture,
	   nullptr,
	   &mRenderRect,
	   mTankAngle,
	   &center,
	   mIsHuman? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    );

    if (mProjectile.mIsPresent) {
	   int angle = static_cast<int>(std::atan(mProjectile.mYVelocity / mProjectile.mXVelocity) * kRadInDegrees);

	   if (mIsHuman) {
		  angle *= -1;
	   }

        SDL_RenderCopyEx(
		  Game::sRenderer,
		  Game::sProjectileTexture,
		  nullptr,
		  &mProjectile.mRenderRect,
		  angle,
		  nullptr,
		  mIsHuman? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
	   );
    }

    int hpbar_x;

    if (mIsHuman) {
        hpbar_x = 50;
    } else {
        hpbar_x = kWindowWidth - 250;
    }

    for (int i = 0; i < mHP * 2; i += 2) {
        SDL_RenderDrawLine(Game::sRenderer, hpbar_x + i, 20, hpbar_x + i, 50);
        SDL_RenderDrawLine(Game::sRenderer, hpbar_x + i + 1, 20, hpbar_x + i + 1, 50);
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

auto Player::GetX() const -> int
{
    return mRenderRect.x;
}

auto Player::GetY() const -> int
{
    return mRenderRect.y;
}

auto Player::GetHP() const -> int
{
    return mHP;
}

auto Player::GetBarrelAngle() const -> double
{
    return mGunBarrelAngle;
}

auto Player::GetDestRect() const -> SDL_Rect
{
    return mRenderRect;
}

void Projectile::Fire(double angle, double barrel_angle, const SDL_Rect& barrel_dest_rect)
{
    if (mTimer.GetTime() < 2000 || this->mIsPresent) {
        return;
    }

    angle *= -1;  // SDL_RenderCopyEx function rotates up if the angle is negative. Here it needs to be positive
    mYVelocity = (angle / 90.0) * kProjectileVelocity;  // If the angle is 90, then all the velocity is on the y axis

    mXVelocity = kProjectileVelocity - mYVelocity; // Tank always fires with the same momentum, so when fired x speed + y speed should always be the same, regardless of the angle

    barrel_angle *= -1;

    mX = barrel_dest_rect.x + kBarrelWidth + 20 - static_cast<int>(std::cos(barrel_angle / kRadInDegrees) * kBarrelWidth);  // Calculates the bullet position
    mY = barrel_dest_rect.y - kBarrelHeight - 10 - static_cast<int>(std::sin(barrel_angle / kRadInDegrees) * kBarrelWidth);

    mRenderRect.w = 32;
    mRenderRect.h = 32;
    mRenderRect.x = static_cast<int>(mX);
    mRenderRect.y = static_cast<int>(mY);

    mIsPresent = true;
    mTimer.StartTiming();
}

void Projectile::Move(bool is_human)
{
    if (mY <= kProjectileHeight || mY >= kWindowHeight - kProjectileHeight ||
	   mX <= kProjectileWidth || mX >= kWindowWidth - kProjectileWidth)
    {  // If the bullet is out of screen bounds
        mIsPresent = false;
        return;
    }

    mYVelocity -= kGravityAcc;  // Decres y velocity by g

    if (is_human) {  // If the human fired the projectile, it goes to right. Bot's projectiles go to left
        mX += mXVelocity;
    } else {
        mX -= mXVelocity;
    }

    mY -= mYVelocity;  // Decreases y coordinate by mYVelocity

    mRenderRect.x = static_cast<int>(mX);
    mRenderRect.y = static_cast<int>(mY);

    if (Game::sHeight - mRenderRect.y <= Game::sTerrain[mRenderRect.x]) {  // If projectile hit the ground
        mIsPresent = false;

	   Game::ProjectileGroundImpact(mRenderRect.x);

        if (!is_human) {  // Determines wether bot should raise or lower the barrel by checking if projectile hit the ground behind or in frot of player
            Game::BotRaiseOrLowerBarrel(mRenderRect.x > Game::sPlayerX);
	   }

        return;
    }

    int enemy_x;
    int enemy_y;

    if (is_human) {
        enemy_x = Game::sBotX;
        enemy_y = Game::sBotY;
    } else {
        enemy_x = Game::sPlayerX;
        enemy_y = Game::sPlayerY;
    }

    if (mRenderRect.x > enemy_x && mRenderRect.x < enemy_x + kTankWidth &&
	   mRenderRect.y > enemy_y && mRenderRect.y < enemy_y + kTankHeight)
    {  // Checks wether the projectile hit the enemy
        mIsPresent = false;
        Game::ReduceHP(!is_human);
	   Game::ProjectileGroundImpact(mRenderRect.x);
    }
}

void Bot::Action(int player_x)  // Determines what should bot do
{
    /* const int FIRE_PAUSE = 2000; */

    /* static clock_t start_time = clock(); */
    /* static clock_t end_time; */

    /* end_time = clock(); */

    if (player_x - mRenderRect.x > kIdealDistance && mRenderRect.x + kTankWidth < kWindowWidth) {
        mRenderRect.x++;
    } else if (player_x - mRenderRect.x < kIdealDistance && mRenderRect.x > 0) {
        mRenderRect.x--;
    }

    if (player_x - mRenderRect.x > kIdealDistance - 50 || player_x - mRenderRect.x < kIdealDistance + 50) {
        this->Fire();

        /* start_time = clock(); */
    }
}
