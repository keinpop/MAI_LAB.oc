#include "realization.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
    int cmd = 0;

    while(scanf("%d", &cmd) != EOF) {
        switch (cmd)
        {
        case 1:
            float A, B, e;
            if (scanf("%f %f %f", &A, &B, &e) == EOF) {
                break;
            }

            puts("Calculate sin(x)");
            printf("%f\n", SinIntegral(A, B, e));
            break;
        case 2: 
            long x;
            if (scanf("%ld", &x) == EOF) {
                break;
            }
            char* str;
            printf("Translation x to binary\n");
            printf("%s\n", str = translation(x));
            free(str);
        default:
            break;
        }
    }

    return 0;
}