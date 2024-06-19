#include "Game.h"

int main(int, char**)
{
    Game::InitGame("Tanks");

    /* const unsigned int FPS = 60; */
    /* const unsigned int frame_delay = 1000 / FPS; */

    while (Game::IsRunning())
    {
        unsigned int frame_time = SDL_GetTicks();

        Game::HandleEvents();
        Game::Update();
        Game::Render();

        frame_time = SDL_GetTicks() - frame_time;

        /* if (frame_delay > frame_time) */
        /*     SDL_Delay(frame_delay - frame_time); */
    }

    Game::Clean();

    return 0;
}
