#include <iostream>
#include <SDL/SDL.h>

#define WIDTH 800
#define HEIGHT 800
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

template<int gsize>
class LifeGrid
{
  public:
    LifeGrid()
    {
        gra = griddata;
        grb = otherdat;
        for (int x = 0; x < gsize; x++) {
            for (int y = 0; y < gsize; y++) {
                gra[y][x] = 0;
            }
        }
    }
    template<int osize>
    void copy(int start[osize][osize], int offx, int offy)
    {
        gra = griddata;
        grb = otherdat;
        for (int x = 0; x < osize; x++) {
            for (int y = 0; y < osize; y++) {
                gra[y+offy][x+offx] = start[y][x];
            }
        }
    }
    void step()
    {
        for (int x = 0; x < gsize; x++) {
            for (int y = 0; y < gsize; y++) {
                int state = gra[y][x], nst, n = neighbours(x, y);
                if (state == 1) {
                    if (n == 2 || n == 3)
                        nst = 1;
                    else
                        nst = 0;
                }
                else if (state == 0) {
                    if (n == 3 || n == 6)
                        nst = 1;
                    else
                        nst = 0;
                }
                float rfac = float(rand()) / RAND_MAX;
                if (rfac < 0.00005)
                    nst = 1 - nst;
                grb[y][x] = nst;
            }
        }
        int (*grtemp)[gsize] = gra;
        gra = grb;
        grb = grtemp;
    }
    int get(int x, int y) { return gra[y][x]; }
    int size() { return gsize; }
  private:
    int griddata[gsize][gsize], otherdat[gsize][gsize];
    int (*gra)[gsize], (*grb)[gsize];

    int neighbours(int x, int y)
    {
        int count = 0;
        for (int dx = -1; dx <= 1; dx++) {
            if (x + dx < 0)
                continue;
            if (x + dx >= gsize)
                break;
            for (int dy = -1; dy <= 1; dy++) {
                if ((dx == 0) && (dy == 0))
                    continue;
                if (y + dy < 0)
                    continue;
                if (y + dy >= gsize)
                    break;
                if (gra[y+dy][x+dx] == 1)
                    count++;
            }
        }
        return count;
    }

};

template <int gsize>
void display(LifeGrid<gsize> &grid)
{
    for (int y = 0; y < grid.size(); y++) {
        for (int x = 0; x < grid.size(); x++) {
            if (grid.get(x, y)== 1)
                std::cout << "oo";
            else if (grid.get(x, y) == 0)
                std::cout << "  ";
        }
        std::cout << "\n";
    }
}

template<int gsize>
void DrawScreen(SDL_Surface* screen, LifeGrid<gsize> &g)
{ 
    int x, y, ytimesw;
          
    if(SDL_MUSTLOCK(screen)) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }

    /*for(y = 0; y < screen->h; y++ ) 
    {
        ytimesw = y*screen->pitch/BPP;
        for( x = 0; x < screen->w; x++ ) 
        {
            setpixel(screen, x, ytimesw, (x*x)/256+3*y+0, (y*y)/256+x+0, 0);
        }
    }*/
    for (int y = 0; y < g.size(); y++) {
        ytimesw = y*screen->pitch/BPP;
        for (int x = 0; x < g.size(); x++) {
            int col = g.get(x, y) * 200;
            setpixel(screen, x, ytimesw, col, col, col);
        }
    }

    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen); 
}
int main()
{
    int grid[6][6] = {{0, 0, 0, 0, 0, 0},
                      {0, 0, 1, 1, 1, 0},
                      {0, 1, 0, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0}};
    LifeGrid<HEIGHT> g;
    g.copy(grid, 100, 100);
    g.copy(grid, 400, 100);
    g.copy(grid, 600, 100);
    g.copy(grid, 700, 100);
    g.copy(grid, 100, 400);
    g.copy(grid, 400, 400);
    g.copy(grid, 600, 400);
    g.copy(grid, 700, 500);
    g.copy(grid, 100, 600);
    g.copy(grid, 400, 600);
    g.copy(grid, 600, 600);
    g.copy(grid, 700, 600);

    SDL_Surface *screen;
    SDL_Event event;

    int keypress = 0, seq = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
    {
        SDL_Quit();
        return 1;
    }

    while(!keypress)
    {
         DrawScreen(screen, g);
         while(SDL_PollEvent(&event))
         {
              switch (event.type)
              {
                  case SDL_QUIT:
                  keypress = 1;
                  break;
                  case SDL_KEYDOWN:
                       //keypress = 1;
                       break;
              }
         }
         g.step();
    }

    SDL_Quit();

    return 0;
}
