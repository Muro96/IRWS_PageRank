#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

//OPEN DATASET AND RETURN FILE
FILE *open_file(char *filename, char *mode)
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

  // The first row always starts at position 0
  row_ptr[0] = 0;
  
  int fromnode, tonode;
  int cur_row = 0;
  int i = 0;
  int j = 0;
  // Elements for row
  int elrow = 0;
  // Cumulative numbers of elements
  int curel = 0;
  
  while(!feof(f)){
    
    fscanf(f,"%d%d",&fromnode,&tonode);

    if (fromnode > cur_row) { // change the row
      curel = curel + elrow;
      for (int k = cur_row + 1; k <= fromnode; k++) {
        row_ptr[k] = curel;
      }
      elrow = 0;
      cur_row = fromnode;
    }
    val[i] = 1.0;
    col_ind[i] = tonode;
    elrow++;
    i++;
  }
  row_ptr[cur_row+1] = curel + elrow - 1;
}

int HITS(int nodes, int edges, float *h, float *val, int *row_ptr, int *col_ind){
  int i = 0;
  int j = 0;
  int looping = 1;
  int k = 0;
  int rowel = 0;
  int curcol = 0;
  float temp = 0.0;
  float h_sum = 0.0;

  for(i=0; i<nodes; i++){
    h[i] = 1.;
  }

  float h_new[nodes];

  for(i=0; i<nodes; i++){
    h_new[i] = 0.;
  }

  while (looping){
    for(i=0; i<nodes; i++){
      rowel = row_ptr[i+1] - row_ptr[i];
      for (j=0; j<rowel; j++) {
        temp = temp + (val[curcol] * h[col_ind[curcol]]);
        curcol++;
      }
      h_new[i] = temp;
      temp = 0.0;
    }

    //normalize
    for(i=0; i<nodes; i++){
      h_sum = h_sum + h_new[i];
    }

    for(i=0; i<nodes; i++){
      h_new[i] = h_new[i]/h_sum;
    }

    // TERMINATION: check if we have to stop
    float error = 0.0;
    for(i=0; i<nodes; i++) {
      error =  error + fabs(h_new[i] - h[i]);
    }

    //if two consecutive instances of pagerank vector are almost identical, stop
    if (error < 0.00001){
      looping = 0;
    }
    
    // Update Hits vector[]
    for (i=0; i<nodes;i++){
        h[i] = h_new[i];
    }

    // Increase the number of iterations anr set everything to 0
    k = k + 1;
    temp = 0.0;
    curcol = 0;
    h_sum = 0.0;
  }

  return k;
}

void sort(float *r, int *t, int n){
  int i,j, temp1;
  float temp2 = 0.0;

  for(i=1;i<n;i++){ 
    temp2=r[i];
    temp1=t[i];
    j=i-1; 
    while(j>=0 && r[j]>temp2){
		  r[j+1]=r[j];
      t[j+1]=t[j];		
		  j--;		
	  }
	  r[j+1]=temp2;
    t[j+1]=temp1;
	  }
}


void Rank(float *r, int *t, int n, float *h, int nodes){
  int i;
  FILE *f;
  char filename[] = "Jaccard.txt";
  f = open_file(filename, "a");

  for(i=0; i<n; i++){
    r[i] = h[i];
    t[i] = i;
  }

  sort(r, t, n);

  for(i=n; i<nodes; i++){
    if (r[0] < h[i]){
      //printf("%d %.16f\n", i, h[i]);
      r[0] = h[i];
      t[0] = i;
      sort(r, t, n);
    }
  }
  fprintf(f, "Hits top-k nodes:\n");
  for(i=n-1; i>=0; i--){
    fprintf(f, "%d\n", t[i]);
    printf("%d   %f\n", t[i], r[i]);
  }  
  
  fclose(f);
}

int main(){
  clock_t begin, end;
  double time_spent;
  FILE *f_a, *f_h;
  int nodes, edges;
  int i, iter_a, iter_h, n;

  begin = clock();

  // OPEN DATASET
  char filename_h[] = "web-NotreDame.txt";
  char filename_a[] = "web-NotreDame-transpose.txt";
  f_h = open_file(filename_h, "r");
  f_a = open_file(filename_a, "r");


  // READ DATASET
  read(f_h, &nodes, &edges);
  read(f_a, &nodes, &edges);
  printf("numero di nodi = %d e archi = %d\n", nodes, edges);

  
  float *val_a = calloc(edges, sizeof(float));
  int *col_ind_a = calloc(edges, sizeof(int));
  int *row_ptr_a = calloc(nodes+1, sizeof(int));

  float *val_h = calloc(edges, sizeof(float));
  int *col_ind_h = calloc(edges, sizeof(int));
  int *row_ptr_h = calloc(nodes+1, sizeof(int));
  
  initialize_CSR(nodes, edges, f_a, val_a, row_ptr_a, col_ind_a);
  initialize_CSR(nodes, edges, f_h, val_h, row_ptr_h, col_ind_h);

  //CREATE RWO VECTORS FOR AUTHORITY AND HUB SOCRES
  float a[nodes];
  float h[nodes];

  iter_a = HITS(nodes, edges, a, val_a, row_ptr_a, col_ind_a);
  iter_h = HITS(nodes, edges, h, val_h, row_ptr_h, col_ind_h);
  
  
  end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  printf ("\nNumber of iteration to converge: %d \n\n", iter_a); 

  printf("\n\nTime spent: %f seconds.\n", time_spent);

  //RANKING PHASE
  printf("Insert number of top k-nodes that you want to visualize :");
  scanf("%d", &n );

  float hits_rank_a[n];
  int top_nodes_a[n];
  float hits_rank_h[n];
  int top_nodes_h[n];

  printf("HITS Hub Rank:\n");
  Rank(hits_rank_a, top_nodes_h,n, h, nodes);
  printf("\n\n");
  printf("HITS Authority Rank:\n");
  Rank(hits_rank_a, top_nodes_a,n, a, nodes);
  return 0;
}