#include <stdio.h>
#include "TSParser.h"

void PrintUsage(const char *pAppPath)
{
    printf("Usage: %s file\n", pAppPath);
    printf("Demo : %s /home/demo.ts\n", pAppPath);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        PrintUsage(argv[0]);
        exit(0);
    }

    TSParser tParser(argv[1]);
    tParser.Parse();

    return 0;
}

