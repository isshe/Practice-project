
#include <stdio.h>

#include <SDL2/SDL.h>
//#include <SDL2/SDL2main.h>


int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Couldn't initialize SDL - %s\n", SDL_GetError());
    }
    else
    {
         printf("Success init SDL\n");
    }

    return 0;
}
