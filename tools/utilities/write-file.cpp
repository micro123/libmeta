#include <cstdio>
#include <cstring>

/*
*  1. output
*  ... lines
*/
int main(int argc, char const *argv[])
{
    if (argc < 2)
        return 1;
    FILE *f = fopen(argv[1], "w");
    if (!f) {
        fprintf(stderr, "cannot open file %s\n", argv[1]);
        return 1;
    }
    for (int i=2; i<argc; ++i) {
        if (strlen(argv[i]) > 2)
            fprintf(f, "%s\n", argv[i]);
    }
    fclose(f);
    return 0;
}
