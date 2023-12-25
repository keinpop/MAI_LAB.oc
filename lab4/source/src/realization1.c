#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "realization.h"

// rectangles method
float SinIntegral(float A, float B, float e)
{
    float step = e;
    int count = (int)((B - A) / step);

    float ans = 0;

    for (int i = 0; i <= count; ++i) {
        ans += step * sinf(A + (float)(i - 1) * step);
    }

    return ans;
}

// convert to binary
char* translation(long x)
{
    char* res = calloc(1, sizeof(char));
    res[0] = '\0';

    if (x == 0) {
        char* tmp = calloc(strlen(res) + 2, sizeof(char));
        strcpy(tmp + 1, res);
        free(res);
        res = tmp;
        res[0] = '0';
    } 

    while (x > 0) {
        char c = (x & 1u) + '0';
        x >>= 1u;
        char* tmp = calloc(strlen(res) + 2, sizeof(char));
        strcpy(tmp + 1, res);
        free(res);
        res = tmp;
        res[0] = c; 
    }

    return res;
}