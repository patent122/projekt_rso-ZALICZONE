#include "shared.h"

void invert_endianness(void *buffer, int n)
{
    int i = 0, j = n - 1;
    unsigned char *p = (unsigned char *)buffer;

    while (i < j)
    {
        unsigned char t = p[i];
        p[i] = p[j];
        p[j] = t;
        i++;
        j--;
    }
}
