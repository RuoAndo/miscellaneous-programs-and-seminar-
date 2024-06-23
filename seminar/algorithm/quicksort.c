#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000

int sort[N];
int counter;

void QuickSort(int bottom,int top,int *data) {
  int lower, upper, div, temp,i;
    if(bottom >= top) {
        return;
    }

    div = data[bottom];
    for(lower = bottom, upper = top; lower < upper;) {
        while(lower <= upper && data[lower] <= div) {
            lower++;
        }
        while(lower <= upper && data[upper] > div) {
            upper--;
        }
        if(lower < upper) {
            temp = data[lower];
            data[lower] = data[upper];
            data[upper] = temp;
        }
    }

    temp = data[bottom];
    data[bottom] = data[upper];
    data[upper] = temp;

    if(counter %1000 == 0)
      {	
	FILE *file;
	char fn[256];

	printf("HIT \n");
	sprintf(fn, "quick-%d",counter);
	file = fopen(fn,"a");

	for (i = 0; i < N; i++)	
	  fprintf(file,"%d\n",sort[i]);
	fclose(file);
      }
	
    counter = counter + 1;

    QuickSort(bottom, upper - 1, data);
    QuickSort(upper + 1, top, data);
}

int main(void) {
  int i,n;

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
    QuickSort(0, N - 1, sort);

    printf("done. %d \n", counter);
    
    return EXIT_SUCCESS;
}
