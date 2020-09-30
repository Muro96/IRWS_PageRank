#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){

  // execution time
  double execution_time;
  clock_t start, end;
  start = clock();

  // Open the data set
  char fname[]="./web-NotreDame.txt";
  FILE *fp;
  if((fp = fopen(fname,"r")) == NULL)
  {
    fprintf(stderr,"[Error] Cannot open the file");
    exit(1);
  }

  

  end = clock();
  execution_time = ((double)(end - start)) / CLOCKS_PER_SEC;

  printf("\nExecution time: %f seconds.\n", execution_time);
  return 0;
}
