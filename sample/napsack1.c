#include <stdio.h>
#include <stdlib.h>

#define N 5
int size[N] = {2, 3, 5, 6, 9};
int value[N] = {2, 4, 7, 10, 14};

#define NAP_SIZE 16

int main(void) {
    int nap_value[NAP_SIZE + 1] = {0};
    int i, j, new_value;

    for(i = 1; i < NAP_SIZE + 1; i++) {
        printf("%2d ", i);
    }
    printf("\n\n");

    for(i = 0; i < N; i++) {
        for(j = size[i]; j < NAP_SIZE + 1; j++) {
            /* 品物iを入れてみた場合，新しい価値はどう変わるか */
            new_value = nap_value[j - size[i]] + value[i];

            if(new_value > nap_value[j]) {
                nap_value[j] = new_value;
            }
        }

        printf("item:%d size:%d value:%d | ", i, size[i], value[i]);
        for(j = 1; j < NAP_SIZE + 1; j++) {
            printf("%2d ", nap_value[j]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}
