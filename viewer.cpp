#include <iostream>
#include <SDL/SDL.h>

#define WIDTH 1000
#define HEIGHT 1000
#define BPP 4
#define DEPTH 32


void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 colour;  
             
    colour = SDL_MapRGB( screen->format, r, g, b );
                  
    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = colour;
}

char encode(int v[8])
{
    int a = 0;
    for (int i = 0; i < 8; i++)
        a |= v[i] << i;
    return a;
}

const int size = 1000;

void DrawScreen(SDL_Surface* screen, int grid[size][size])
{ 
    int x, y, ytimesw;
          
    if(SDL_MUSTLOCK(screen)) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }

    for (int y = 0; y < size; y++) {
        int ytimesw = y*screen->pitch/BPP;
        for (int x = 0; x < size; x++) {
            int col = grid[y][x] * 200;
            setpixel(screen, x, ytimesw, col, col, col);
        }
    }

    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen); 
}
int main(int argc, const char**argv)
{
    if (argc < 2)
        return -1;

    int grid[size][size];
    int x = 0, y = 0;
    
    FILE *fp;
    char ch;
    if((fp = fopen(argv[ 1 ],"r"))==NULL) {
        printf("Cannot open file.\n");
        exit(1);
    }
    while(ch != EOF) {
        ch = fgetc( fp );
        if (ch == '\n') {
            x = 0;
            y++;
            continue;
        }
        else if (ch == 'o')
            grid[x][y] = 1;
        else if (ch == ' ')
            grid[x][y] = 0;
        x++;
        if (x >= size)
            continue;
    }
    fclose(fp);

    /*for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++)
            std::cout << grid[y][x];
        std::cout << "\n";
    }*/
    
    SDL_Surface *screen;
    SDL_Event event;

    int keypress = 0, seq = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
    {
        SDL_Quit();
        return 1;
    }

    if (argc == 3)
        keypress = 1;
    do
    {
         DrawScreen(screen, grid);
         while(SDL_PollEvent(&event))
         {
              switch (event.type)
              {
                  case SDL_QUIT:
                  keypress = 1;
                  break;
                  case SDL_KEYDOWN:
                       keypress = 1;
                       break;
              }
         }
    }while(!keypress);

    if (argc == 3)
        SDL_SaveBMP(screen, argv[2]);

    SDL_Quit();

    return 0;
}
