#include "Player.h"

Player::Player()
{
    tank_angle = 0;
    gun_barrel_angle = 0;

    dest_rect.w = TANK_WIDTH;
    dest_rect.h = TANK_HEIGHT;

    dest_rect.x = WINDOW_WIDTH / 4;

    barrel_dest_rect.w = BARREL_WIDTH;
    barrel_dest_rect.h = BARREL_HEIGHT;

    is_human = true;
    hp = MAX_HP;
}

Player::~Player()
{
    
}

Bot::Bot()
{
    dest_rect.x = 3 * WINDOW_WIDTH / 4;

    barrel_dest_rect.w = BARREL_WIDTH;
    barrel_dest_rect.h = BARREL_HEIGHT;

    is_human = false;
}

void Player::Update()
{
    dest_rect.y = Game::height - Game::terrain[dest_rect.x] - TANK_HEIGHT;  // Calculates y position from terrain height at tank's x position

    int x2 = dest_rect.x + std::cos(tank_angle / RAD_IN_DEGREES) * (double)TANK_WIDTH;  // Calculates 2nd x position (the other end of the tank) by tank angle
    int y2 = Game::height - Game::terrain[x2] - TANK_HEIGHT;  // Calculates 2nd y position based on height at x2

    tank_angle = -std::asin( static_cast<double>(POS_Y - y2) / TANK_WIDTH  ) * RAD_IN_DEGREES;  // Calculates tank angle using asin of sin(angle)
                                                                                                // sin(angle) = opposite / hypothenuse

    // Calculates positions using circle coordinate formulas for x and y. Calculations are not fully precise for some reason. They are more precise with BARREL_X + 7
    barrel_dest_rect.x = dest_rect.x + BARREL_X * 2 - (long double)(BARREL_X + 7) * std::cos(std::abs(tank_angle) / RAD_IN_DEGREES);
    barrel_dest_rect.y = dest_rect.y + BARREL_Y + (long double)(BARREL_X) * std::sin(tank_angle / RAD_IN_DEGREES);

    if (!is_human)  // Bot is flipped horizontally, so barrel needs to be moved
        barrel_dest_rect.x -= BARREL_X - 3;

    if (bullet.is_present)
        bullet.Move(is_human);
}

void Player::Render()
{
    SDL_Point center;

    center.x = (is_human? 0 : BARREL_WIDTH);
    center.y = BARREL_HEIGHT;
    SDL_RenderCopyEx(Game::renderer, Game::barrel_texture, nullptr, &barrel_dest_rect, gun_barrel_angle + tank_angle, &center, is_human? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

    center.x = 0;
    center.y = BARREL_Y;
    SDL_RenderCopyEx(Game::renderer, Game::tank_texture, nullptr, &dest_rect, tank_angle, &center, is_human? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

    if (bullet.is_present)
        SDL_RenderCopyEx(Game::renderer, Game::projectile_texture, nullptr, &bullet.dest_rect, 0, nullptr, is_human? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

    int x_start;  // Code below renders hp bars. For human, bar starts from 50px. For bot it starts from WINDOW_WIDTH - 250

    if (is_human)
        x_start = 50;
    else
        x_start = WINDOW_WIDTH - 250;

    for (int i = 0; i < hp * 2; i += 2) {
        SDL_RenderDrawLine(Game::renderer, x_start + i, 20, x_start + i, 50);
        SDL_RenderDrawLine(Game::renderer, x_start + i + 1, 20, x_start + i + 1, 50);
    }
}

void Player::Fire()
{
    bullet.Fire(gun_barrel_angle + tank_angle, gun_barrel_angle, barrel_dest_rect);
}

void Player::ReduceHP()
{
    hp -= 5;
}

SDL_Rect Player::GetDestRect()
{
    return dest_rect;
}

void Player::IncrementX(int val_to_add)
{
    dest_rect.x += val_to_add;
    barrel_dest_rect.x += val_to_add;
}

void Player::DecrementX(int val_to_take)
{
    dest_rect.x -= val_to_take;
    barrel_dest_rect.x -= val_to_take;
}

void Player::IncrementGunBarrelAngle()
{
    gun_barrel_angle++;
}

void Player::DecrementGunBarrelAngle()
{
    gun_barrel_angle--;
}

int Player::GetX()
{
    return dest_rect.x;
}

int Player::GetY()
{
    return dest_rect.y;
}

int Player::GetHP()
{
    return hp;
}

double Player::GetBarrelAngle()
{
    return gun_barrel_angle;
}

void Projectile::Fire(double angle, double barrel_angle, const SDL_Rect& barrel_dest_rect)
{
    if (timer.GetTime() < 2000 || this->is_present) 
        return;    

    angle *= -1;  // SDL_RenderCopyEx function rotates up if the angle is negative. Here it needs to be positive
    y_velocity = (angle / 90.0) * PROJECTILE_VELOCITY;  // If the angle is 90, then all the velocity is on the y axis

    x_velocity = PROJECTILE_VELOCITY - y_velocity; // Tank always fires with the same momentum, so when fired x speed + y speed should always be the same, regardless of the angle

    barrel_angle *= -1;

    dest_rect.w = 32;
    dest_rect.h = 32;
    dest_rect.x = x = barrel_dest_rect.x + BARREL_WIDTH + 20 - std::cos(barrel_angle / RAD_IN_DEGREES) * (double)BARREL_WIDTH;  // Calculates the bullet position
    dest_rect.y = y = barrel_dest_rect.y - BARREL_HEIGHT - 10 - std::sin(barrel_angle / RAD_IN_DEGREES) * (double)BARREL_WIDTH;

    is_present = true;
    timer.StartTiming();
}

void Projectile::Move(bool is_human)
{
    if (y <= PROJECTILE_HEIGHT || y >= WINDOW_HEIGHT - PROJECTILE_HEIGHT || x <= PROJECTILE_WIDTH || x >= WINDOW_WIDTH - PROJECTILE_WIDTH) {  // If the bullet is out of screen bounds
        is_present = false;
        return;
    }

    y_velocity -= GRAVITY_ACC;  // Decres y velocity by g

    if (is_human)  // If the human fired the projectile, it goes to right. Bot's projectiles go to left
        x += x_velocity;
    else
        x -= x_velocity;

    y -= y_velocity;  // Decreases y coordinate by y_velocity

    dest_rect.x = x;
    dest_rect.y = y;

    if (Game::height - dest_rect.y <= Game::terrain[dest_rect.x]) {  // If projectile hit the ground
        is_present = false;

        Game::terrain[dest_rect.x] -= std::sin(45 / RAD_IN_DEGREES) * 30;

        for (int i = 1; i < 45; i++) {  // Reduces height of the terrain circulary
            Game::terrain[dest_rect.x + (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
            Game::terrain[dest_rect.x - (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
        }

        for (int i = 0; i < WINDOW_WIDTH - 1; i++) {  // Smoothes the terrain a little bit
            if (Game::terrain[i + 1] > Game::terrain[i])
                Game::terrain[i + 1] = Game::terrain[i] + 1;
            else if (Game::terrain[i + 1] < Game::terrain[i])
                Game::terrain[i + 1] = Game::terrain[i] - 1;
        }

        if (!is_human) {  // Determines wether bot should raise or lower the barrel by knowing if projectile hit the ground behind or in frot of player
            Game::BotRaiseOrLowerBarrel(dest_rect.x > Game::player_x? true : false);
        }

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

    if (dest_rect.x > enemy_x && dest_rect.x < enemy_x + TANK_WIDTH && dest_rect.y > enemy_y && dest_rect.y < enemy_y + TANK_HEIGHT) {  // Checks wether the projectile hit the enemy
        is_present = false;

        Game::ReduceHP(!is_human);

        Game::terrain[dest_rect.x] -= std::sin(45 / RAD_IN_DEGREES) * 30;  // This code is the same as the one above

        for (int i = 1; i < 45; i++) {
            Game::terrain[dest_rect.x + (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
            Game::terrain[dest_rect.x - (45 - i)] -= std::sin(i / RAD_IN_DEGREES) * 30;
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
    const int FIRE_PAUSE = 2000;

    static clock_t start_time = clock();
    static clock_t end_time;

    end_time = clock();

    if (player_x - dest_rect.x > IDEAL_DISTANCE && dest_rect.x + TANK_WIDTH < WINDOW_WIDTH)
        dest_rect.x++;
    else if (player_x - dest_rect.x < IDEAL_DISTANCE && dest_rect.x > 0)
        dest_rect.x--;

    if (player_x - dest_rect.x > IDEAL_DISTANCE - 50 || player_x - dest_rect.x < IDEAL_DISTANCE + 50) {
        this->Fire();

        start_time = clock();
    }
}