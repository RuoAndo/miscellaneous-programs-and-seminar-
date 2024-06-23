#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000

int sort[N];
int buffer[N];
int counter;

void MergeSort(int n,int x[]) {
    int i, j, k, m;
    if(n <= 1) {
        return;
    }
    m = n / 2;

    MergeSort(m, x);
    MergeSort(n - m, x + m);

    for(i = 0; i < m; i++) {
        buffer[i] = x[i];
    }
    j = m;
    i = k = 0;
    while(i < m && j < n) {
        if(buffer[i] <= x[j]) {
            x[k++] = buffer[i++];
        } else {
            x[k++] = x[j++];
        }
    }
    while(i < m) {
        x[k++]=buffer[i++];
    }

	if(counter %500 == 0)
	  {	
	    FILE *file;
	    char fn[256];

	    sprintf(fn, "merge-%d",counter);
	    file = fopen(fn,"a");

	    for (i = 0; i < N; i++)	
	      fprintf(file,"%d\n",sort[i]);
	    fclose(file);
	  }
	
	counter = counter + 1;
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

    /*
    for(i = 0; i < N; i++) {
        printf("%d ",sort[i]);
    }
    */

    counter = 0;
    
    // BubbleSort();
    MergeSort(N, sort);
    
    printf("done. %d \n", counter);

    for(i = 0; i < N; i++) {
        printf("%d ",sort[i]);
    }
	
    /*
    for(i=0;i<N;i++) {
        printf("%d ",sort[i]);
    }

    return EXIT_SUCCESS;
    */
}
