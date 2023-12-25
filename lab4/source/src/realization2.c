#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "realization.h"

// trapezoid method
float SinIntegral(float A, float B, float e)
{
    int n = (int)((B - A) / e);
    float range = (B - A) / (float)n;
    float sinDiap = sinf(A) + sinf(B);// sinus diapason function's

    for (int i = 1; i < n; ++i) {
        sinDiap += 2 * sinf(A + ((float)i * range));
    }

    return (range * sinDiap) / 2;
}

// convert to ternary
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
        char c = (x % 3) + '0';
        x /= 3;

        char* tmp = calloc(strlen(res) + 2, sizeof(char));
        strcpy(tmp + 1, res);
        free(res);
        res = tmp;
        res[0] = c;
    }

    return res;
}