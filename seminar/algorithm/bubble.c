#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000

int sort[N];
int counter;

void BubbleSort(void) {
    int i, j, flag;

    do {
        flag = 0;
        for(i = 0; i < N - 1; i++) {
            if(sort[i] > sort[i + 1]) {
                flag = 1;
                j = sort[i];
                sort[i] = sort[i + 1];
                sort[i + 1] = j;
            }
        }

	if(counter %1000 == 0)
	  {	
	    FILE *file;
	    char fn[256];

	    sprintf(fn, "bubble-%d",counter);
	    file = fopen(fn,"a");

	    for (i = 0; i < N; i++)	
	      fprintf(file,"%d\n",sort[i]);
	    fclose(file);
	  }
	
	counter = counter + 1;
	
    } while(flag == 1);
}

int main(void) {
  int i,n;

    srand((unsigned int)time(NULL));


    float siny[10000];

    FILE *fp;

    fp = fopen("tmp", "r");
    if(fp == NULL) {
      printf("ファイルを開くことが出来ませんでした．\n");
      return;
    }

    n = 0;

    while ( ! feof(fp) && n < 10000) {
      fscanf(fp, "%d", &(sort[n]));
      n++;
    }

    fclose(fp);

    for(i = 0; i < N; i++) {
        printf("%d ",sort[i]);
    }

    counter = 0;
    
    BubbleSort();

    printf("done. %d \n", counter);
    
    /*
    for(i=0;i<N;i++) {
        printf("%d ",sort[i]);
    }

    return EXIT_SUCCESS;
    */
}
