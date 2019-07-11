#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char ** argv)
{
    int opt;
    while((opt = getopt(argc, argv, "ht:v")) != -1){
        switch (opt) {
        case 'h':
            printf("help\n");
            return 0;
        case 't':
            printf("%s\n", optarg);
            break;
        case 'v':
            printf("version\n");
            return 0;
        default:
            printf("other option\n");
            return 2;
            break;
        }
    }

    printf("%d\n", optind);

    optind = 2;
    const char* const* cmdLine = argv + optind;
    printf("%s\n", *cmdLine);

    return 0;
}