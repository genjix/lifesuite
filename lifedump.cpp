#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
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

char encode(int *v)
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
    void copy(const int start[osize][osize], int offx, int offy)
    {
        gra = griddata;
        grb = otherdat;
        for (int x = 0; x < osize; x++) {
            for (int y = 0; y < osize; y++) {
                gra[y+offy][x+offx] = start[y][x];
            }
        }
    }
    bool load(const char* fname)
    {
        int x = 0, y = 0;
        FILE *fp;
        char ch;
        if ((fp = fopen(fname,"r")) == NULL) {
            return false;
        }
        while(ch != EOF) {
            ch = fgetc( fp );
            if (ch == '\n') {
                x = 0;
                y++;
                continue;
            }
            else if (ch == 'o')
                gra[x][y] = 1;
            else if (ch == ' ')
                gra[x][y] = 0;
            x++;
            if (x >= gsize)
                continue;
        }
        fclose(fp);
        return true;
    }
    void save()
    {
        time_t rawtime;
        time(&rawtime);
        std::string fname = "dump/";
        fname += ctime(&rawtime);
        fname = std::string(fname.begin(), fname.end() - 4);

        FILE *f = fopen(fname.c_str(), "w");
        for (int y = 0; y < gsize; y++) {
            for (int x = 0; x < gsize; x++) {
                if (gra[y][x] == 1)
                    fprintf(f, "o");
                else
                    fprintf(f, " ");
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }
    void placereplicator()
    {
        const int repl[6][6] =
            {{0, 0, 0, 0, 0, 0},
             {0, 0, 1, 1, 1, 0},
             {0, 1, 0, 0, 0, 0},
             {0, 1, 0, 0, 0, 0},
             {0, 1, 0, 0, 0, 0},
             {0, 0, 0, 0, 0, 0}};

        float rfac = float(rand()) / RAND_MAX;
        int x = rfac * gsize;
        rfac = float(rand()) / RAND_MAX;
        int y = rfac * gsize;
        copy(repl, x, y);
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
                {
                    nst = 1 - nst;
                    if (rfac < 0.0000001)
                        placereplicator();
                }
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

const int size = 1000;

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

void *display(void *ptr)
{
    LifeGrid<size> *g;
    g = (LifeGrid<size>*)ptr;

    SDL_Surface *screen;
    SDL_Event event;

    int keypress = 0, seq = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return NULL;

    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
    {
        SDL_Quit();
        return NULL;
    }

    while(!keypress)
    {
         DrawScreen(screen, *g);
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
    }

    SDL_Quit();
}

void *querysave(void* ptr)
{
    LifeGrid<size> *g;
    g = (LifeGrid<size>*)ptr;
    char value;
    while(1)
    {
        std::cin >> value;
        if (value == 's')
            g->save();
        else if (value == 'd') {
            pthread_t thread;
            pthread_create(&thread, NULL, display, ptr);
        }
    }
}
void *periodsave(void* ptr)
{
    LifeGrid<size> *g;
    g = (LifeGrid<size>*)ptr;
    char value;
    while(1)
    {
        g->save();
        sleep(60);
    }
}

int main(int argc, const char **argv)
{
    LifeGrid<size> g;
    if (argc < 2) {
        for (int i = 0; i < 2000; i++)
            g.placereplicator();
    }
    else {
        if (!g.load(argv[1])) {
            std::cerr << "Error loading file\n";
            return -1;
        }
    }

    pthread_t thread;
    pthread_create(&thread, NULL, querysave, (void*)&g);
    pthread_t thread1;
    pthread_create(&thread1, NULL, periodsave, (void*)&g);
    
    while(1)
    {
        //g.save();
        //g.step();
    }

    return 0;
}
