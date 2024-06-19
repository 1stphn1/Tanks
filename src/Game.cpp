#include "Game.h"

const char* background_pictures[NUMBER_OF_PICTURES] = {
    "assets/background1.png",
    "assets/background2.jpg",
    "assets/background3.jpg",
    "assets/background4.webp",
    "assets/background5.jpg",
    "assets/background6.jpg",
    "assets/background7.jpg"
};

bool Game::InitGame(const char *window_title)
{
    width  = WINDOW_WIDTH;  // TODO: make it get width and height from system resolution
    height = WINDOW_HEIGHT;

    running = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LOG("error at SDL_Init");
        PrintError();
        return false;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        LOG("error at IMG_Init");
        PrintError();
        return false;
    }

    window = SDL_CreateWindow(window_title, 200, 200, width, height, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        PrintError();
        return false;
    }

    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr) {
        PrintError();
        return false;
    }

    SDL_SetRenderDrawColor(renderer, 0xe0, 0xf, 0xf, 0xff);  // Sets to red for rendering health bars

    srand(time(nullptr));

    ground_texture     = LoadTexture("assets/ground.jpg");
    surfacelay_texture = LoadTexture("assets/surface.png");
    barrel_texture     = LoadTexture("assets/barrel.png");
    tank_texture       = LoadTexture("assets/tank.png");
    projectile_texture = LoadTexture("assets/projectile.png");
    background_texture = LoadTexture(background_pictures[rand() % NUMBER_OF_PICTURES]);

    Noise::GenerateTerrain();

    for (int i = 0; i < WINDOW_WIDTH - 1; i++) {  // Smoothens the terrain a little bit
        if (Game::terrain[i + 1] > Game::terrain[i])
            Game::terrain[i + 1] = Game::terrain[i] + 1;
        else if (Game::terrain[i + 1] < Game::terrain[i])
            Game::terrain[i + 1] = Game::terrain[i] - 1;
    }

    for (int i = 1; i < WINDOW_WIDTH - 1; i++) {  // Smoothens the terrain, but different pairs
        if (Game::terrain[i + 1] > Game::terrain[i])
            Game::terrain[i + 1] = Game::terrain[i] + 1;
        else if (Game::terrain[i + 1] < Game::terrain[i])
            Game::terrain[i + 1] = Game::terrain[i] - 1;
    }

    player = Player();
    bot = Bot();

    running = true;
    return true;
}

void Game::Clean()
{
    running = false;

    SDL_DestroyTexture(background_texture);
    SDL_DestroyTexture(surfacelay_texture);
    SDL_DestroyTexture(projectile_texture);
    SDL_DestroyTexture(ground_texture);
    SDL_DestroyTexture(barrel_texture);
    SDL_DestroyTexture(tank_texture);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    IMG_Quit();
}

void Game::HandleEvents()
{
    SDL_Event event;
    bot.Action(player.GetX());

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            return;
        }

        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                if (player.GetX() >= 3)
                    player.DecrementX(3);
                break;
            case SDLK_RIGHT:
                if (player.GetX() + TANK_WIDTH <= WINDOW_WIDTH - 3)
                    player.IncrementX(3);
                break;
            case SDLK_UP:
                if (player.GetBarrelAngle() > -65)
                   player.DecrementGunBarrelAngle();
                break;
            case SDLK_DOWN:
                if (player.GetBarrelAngle() < 5)
                    player.IncrementGunBarrelAngle();
                break;
            case SDLK_SPACE:
                player.Fire();
                break;
            case SDLK_ESCAPE:
                Clean();
                break;
        }
    }
}

void Game::Update()
{
    if (player.GetHP() <= 0) {
        LOG("You lost");
        running = false;
        return;
    } else if (bot.GetHP() <= 0) {
        LOG("You won");
        running = false;
        return;
    }

    player_x = player.GetX();
    player_y = player.GetY();
    bot_x = bot.GetX();
    bot_y = bot.GetY();

    player.Update();  
    bot.Update(); 
}

void Game::Render()
{
    SDL_RenderClear(renderer);

    RenderMap();
    player.Render();
    bot.Render();

    SDL_RenderPresent(renderer);
}

bool Game::IsRunning()
{
    return running;
}

SDL_Renderer *Game::GetRenderer()
{
    return renderer;
}

void Game::RenderMap()
{
    SDL_RenderCopy(renderer, background_texture, nullptr, nullptr);

    const int ground_image_width  = 1600;
    const int ground_image_height = 800;

    SDL_Rect src_rect, dst_rect;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = 1;
    src_rect.h = ground_image_height;

    dst_rect.x = 0;
    dst_rect.w = 1;
    dst_rect.h = ground_image_height;

    for (int i = 0; i < WINDOW_WIDTH; i++) {  // This code renders terrain pixel by pixel. That's why src_rect.x increases by 1 every iteration        
        dst_rect.y = WINDOW_HEIGHT - terrain[i];  // Calculates y from terrain height
        src_rect.y = ground_image_height - terrain[i];  // Calculates src_rect.y so the texture would't stretch
        dst_rect.h = terrain[i];

        SDL_RenderCopy(renderer, ground_texture, &src_rect, &dst_rect);

        dst_rect.x++;

        if (++src_rect.x == ground_image_width) {   // Starts rendering texture from begging if the end of the texture is reached
            src_rect.x = 0;
        }
    }

    SDL_Rect dst_rect_surface_layer;

    dst_rect_surface_layer.x = 0;
    dst_rect_surface_layer.y = 0;
    dst_rect_surface_layer.w = 20;
    dst_rect_surface_layer.h = 20;

    for (int i = 0; i < WINDOW_WIDTH; i++) {  // Renders surface layer
        dst_rect_surface_layer.y = WINDOW_HEIGHT - terrain[i];

        if (i != 0 && 
		  (WINDOW_HEIGHT - terrain[i - 1]) - (WINDOW_HEIGHT - terrain[i]) <= 5  &&
		  (WINDOW_HEIGHT - terrain[i - 1]) - (WINDOW_HEIGHT - terrain[i]) >= -5)
	   {
            dst_rect_surface_layer.y = WINDOW_HEIGHT - terrain[i - 1];
        }

        SDL_RenderCopy(renderer, surfacelay_texture, nullptr, &dst_rect_surface_layer);
        dst_rect_surface_layer.x++;
    }
}

void Game::BotRaiseOrLowerBarrel(bool raise)
{
    if (raise && bot.GetBarrelAngle() < 5)
        bot.IncrementGunBarrelAngle();
    else if (bot.GetBarrelAngle() > -65)
        bot.DecrementGunBarrelAngle();
}

void Game::ReduceHP(bool to_human)
{
    if (to_human)
        player.ReduceHP();
    else
        bot.ReduceHP();
}

SDL_Texture *LoadTexture(const char *file_path)  // Returns null on failure
{
    SDL_Texture *texture = IMG_LoadTexture(Game::renderer, file_path);

    if (texture == nullptr)
	   LOG("texture is null in LoadTexture");

    return texture;
}

double Distance(int x1, int y1, int x2, int y2)
{
    return std::sqrt( std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2) );
}

float Noise::RandomFloat() 
{
    static std::default_random_engine e(time(NULL));
    static std::uniform_real_distribution<float> u(0, 1);
    return u(e);
}

float Noise::Noise(float x)
{
    int x0 = floor(x);
    /* int x1 = x0 + 1; */
    float t = x - x0;
    float v0 = RandomFloat();
    float v1 = RandomFloat();
    float noise = (1 - t) * v0 + t * v1;
    return noise;
}

float Noise::Lerp(float a, float b, float t) 
{
    return (1 - t) * a + t * b;
}

void Noise::GenerateTerrain() 
{
    float range = 0.5;
    float frequency = 0.05;
    float amplitude = 1;
    float persistence = 0.5;

    for (int i = 0; i < WINDOW_WIDTH; i++) {
        float total = 0;
        float freq = frequency;
        float amp = amplitude;

        for (int j = 0; j < 4; j++) {
            total += Noise(i * freq) * amp;
            freq *= 2;
            amp *= persistence;
        }

        Game::terrain[i] = std::round(Lerp(0, total, (float)WINDOW_HEIGHT / 2) * range);
    }
}
