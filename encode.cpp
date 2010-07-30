#include <iostream>
#include <time.h>

char encode(int v[8])
{
    int a = 0;
    for (int i = 0; i < 8; i++)
        a |= v[i] << i;
    return a;
}

int main()
{
    time_t secs = time(NULL);
    const time_t wait = 60*60*15;   // 15 mins
    std::cout << secs << "\n";
    //          1  2  4  8 16 32 64 128
    int v[8] = {0, 1, 0, 1, 1, 0, 0, 0};
    std::cout << int(encode(v)) << "\n";
    return 0;
}
