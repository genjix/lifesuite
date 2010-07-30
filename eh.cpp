#include <unistd.h>

template<int gsize>
class LifeGrid
{
  public:
    LifeGrid()
    {
    }
  private:
    int griddata[gsize][gsize], otherdat[gsize][gsize];
};

int main()
{
    sleep(10);
    
    return 0;
}
