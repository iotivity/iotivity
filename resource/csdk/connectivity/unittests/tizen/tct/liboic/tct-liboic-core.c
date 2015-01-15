#include <stdio.h>
#include <string.h>
#include "tct-liboic-core.h"

int main(int argc, const char* argv[])
{
    int result = -1;
    int i;

    if (argc != 2) {
        printf("Usage: %s <testcase name>\n", argv[0]);
        return 2;
    }

    for (i = 0; tc_array[i].name; i++) {
        if (!strcmp(argv[1], tc_array[i].name)) {
            if (tc_array[i].startup)
                tc_array[i].startup();

            result = tc_array[i].function();

            if (tc_array[i].cleanup)
                tc_array[i].cleanup();

            return result;
        }
    }

    printf("Unknown testcase name: \"%s\"\n", argv[1]);
    return 2;
}
