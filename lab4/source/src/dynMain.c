#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef enum {
    FIRST,
    SECOND,
} CONTEXT;

CONTEXT r = FIRST;

const char* libName1 = "../cmake-build-debug/libfirst.so";
const char* libName2 = "../cmake-build-debug/libsecond.so";

float (*sinInt)(float, float, float) = NULL; // Pointer to sinus integral function
char* (*translation)(long x) = NULL; // Pointer to translation function
char* err;

void* libHandle = NULL;

// function to loaded dynamic library
void loadDynLib(CONTEXT con)
{
    const char* name;

    if (con == FIRST) {
        name = libName1;
    } else if (con == SECOND) {
        name = libName2;
    } else {
        puts("Error enter! exit 1");
        exit(EXIT_FAILURE);
    }

    libHandle = dlopen(name, RTLD_LAZY);

    if (!libHandle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

// completion function from dynamic library
void unloadDynLub()
{
    dlclose(libHandle);
}

// functions for loading into a dynamic library
void loadContext()
{
    loadDynLib(r);
    sinInt = dlsym(libHandle, "SinIntegral");
    translation = dlsym(libHandle, "translation");

    if ((err = dlerror())) {
        fprintf(stderr, "%s\n", err);
        exit(EXIT_FAILURE);
    }
}

// context change function
void changeContext()
{
    unloadDynLub();

    if (r == FIRST) {
        r = SECOND;
    } else {
        r = FIRST;
    }

    loadContext();
}

void doc2use()
{
    printf("\tWelcome to programm!\n");
    printf("To work with the program you can use 3 commands:\n");
    printf("\t|0| change the contract\n");
    printf("\t|1| use first function (and more arguments)\n");
    printf("\t|2| use second function (and more arguments)\n");
}

int main()
{
    r = FIRST;
    loadContext();

    int cmd = 0;

    doc2use();
    while(scanf("%d", &cmd) != EOF) {
        switch (cmd)
        {
        case 0:
            changeContext();
            puts("Contract was changed!");
            if (r == FIRST) {
                puts("\tNOW CONTEXT IS FIRST");
            } else {
                puts("\tNOW CONTEXT IS SECOND");
            }
            break;
        case 1:
            float A, B, e;
            if (scanf("%f %f %f", &A, &B, &e) == EOF) {
                break;
            }
            printf("%f\n", sinInt(A, B, e));
            break;
        case 2:
            long x;
            if (scanf("%ld", &x) == EOF) {
                break;
            }

            char* str;
            printf("Translate decimal number '%ld' to ", x);

            if (r == FIRST) {
                printf("binary\n");
            } else {
                printf("ternary\n");
            }

            str = translation(x);
            printf("\t Result: %s\n", str);
            free(str);
        default:
            break;
        }
    }

    unloadDynLub();
    
    return 0;
}