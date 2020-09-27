#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


//OPEN DATASET AND RETURN FILE
FILE *open(char *filename, char *mode)
{
  FILE *f;
  f = fopen(filename, mode);
  if (f == NULL)
  {
    fprintf(stderr, "[Error] Cannot open the file");
    exit(1);
  }
  return f;
}

//READ DATASET AND RETURN NUMBER OF NODES
void read(FILE *f, int *nodes, int *edges)
{
  char ch;
  char str[100];
  ch = getc(f);
  while (ch == '#')
  {
    fgets(str, 100 - 1, f);
    sscanf(str, "%*s %d %*s %d", nodes, edges);
    ch = getc(f);
  }
  ungetc(ch, f);
  *nodes = *nodes;
  *edges = *edges;
}

void initialize_CSR(int nodes, int edges, FILE *f, float *val, int *row_ptr, int *col_ind){
  int fromnode, tonode;
  int cur_row = 0;
  int i = 0;
  int j = 0;
  // Elements for row
  int elem_row = 0;
  // Cumulative numbers of elements
  int curel = 0;

  row_ptr[0] = 0;

  while (!feof(f)){
    fscanf(f, "%d%d", &fromnode, &tonode);
    // DEBUG: print fromnode and tonode
    //printf("From: %d To: %d\n",fromnode, tonode);

    // CHECK IF WE NEED TO CHANGE THE ROW
    if (fromnode > cur_row)
    { // change the row
      curel = curel + elem_row;
      for (int k = cur_row + 1; k <= fromnode; k++)
      {
        // TELLS WHERE START THE ROW (IT'S THE ELEM+1 OF THE PREVIOUS ROW)
        row_ptr[k] = curel;
      }

      elem_row = 0;
      //SET THE CURRENT_ROW WITH THE VALUE OF fromnode
      cur_row = fromnode;
    }
    //SET i-th OF VALUE VECTOR TO 1.0
    val[i] = 1.0;
    //SET THE i-th OF COLUMN INDEX ARRAY TO THE VALUE OF TONODE
    col_ind[i] = tonode;
    //INCREASE ELEMENT THAT WE HAVE VISITED PER ROW
    elem_row++;
    i++;
  }
  row_ptr[cur_row + 1] = curel + elem_row - 1;
}





 void insertionSort(int val[],int edges){
   int i;
   int j;
   float temp;
    for(i=0;i<edges-1;i++){ 
      temp=val[i]; 
      j=i-1; 
      while(j>=0 && val[j]>temp){
        val[j+1]=val[j];		
        j--;		
      }
      val[j+1]=temp;
      }

      for(i=0;i<edges-1;i++){
		    printf("%d \t", val[i]);
	    }
 }

void transposed_CSR(float *val, int *row_ptr, int *col_ind,float *val1,int *row_ptr1, int *col_ind1){
    insertionSort(val,45);
    

}






int main(){
    clock_t begin, end;
    double time_spent;
    FILE *f;
    int nodes, edges;
    int i;
    begin = clock();

    // Set the damping factor 'd'
    //float d = 0.85;

    // OPEN DATASET
    char filename[] = "web-NotreDame.txt";
    f = open(filename, "r");

    // READ DATASET
    read(f, &nodes, &edges);
    printf("numero di nodi = %d e archi = %d", nodes, edges);


    float *val = calloc(edges, sizeof(float));
    int *col_ind = calloc(edges, sizeof(int));
    int *row_ptr = calloc(nodes + 1, sizeof(int));

    int *val1 = calloc(edges, sizeof(float));
    int *col_ind1 = calloc(edges, sizeof(int));
    int *row_ptr1 = calloc(nodes + 1, sizeof(int));

    initialize_CSR(nodes, edges, f, val, row_ptr, col_ind);
    transposed_CSR(val,row_ptr,col_ind,val1,row_ptr1,col_ind1);


}