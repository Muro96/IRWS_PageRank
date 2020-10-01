#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>

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

// CALCULATE IN-DEGREE VALUE FOR EVERY NODE
void In_Degree(float *v, int nodes, int *row_ptr){
    int i, j;
    float rowel = 0.0;

    for(i=0; i<nodes; i++){
      rowel = row_ptr[i+1] - row_ptr[i];
      v[i] = rowel/nodes;
    }
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

// RANK THE TOP-K NODES
void Top_k_Rank (float *r, int *t, float *in, int nodes, int n){
    int i;
    FILE *f;
    char filename[] = "Jaccard.txt";
    f = open_file(filename, "a");

    for(i=0; i<n; i++){
        r[i] = in[i];
        t[i] = i;

    }

    sort(r, t, n);

    for(i=n; i<nodes; i++){
        if (r[0] < in[i]){
            r[0] = in[i];
            t[0] = i;
            sort(r, t, n);
        }
    }
    fprintf(f, "In-Degree top-k nodes:\n");
    for(i=n-1; i>=0; i--){
        fprintf(f, "%d\n", t[i]);
        printf("%d   %f\n", t[i], r[i]);
    } 
}

int main(){
    clock_t begin, end;
    float time_spent;
    FILE *f;
    int nodes, edges;
    int i, n;

    begin = clock();

    // OPEN DATASET
    char filename[] = "web-NotreDame-transpose.txt";
    f = open_file(filename, "r");

    // READ DATASET
    read(f, &nodes, &edges);
    printf("numero di nodi = %d e archi = %d\n", nodes, edges);

    // ISTANCE FOR CREATE THE TWO CSR MATRIX
    float *val = calloc(edges, sizeof(float));
    int *col_ind = calloc(edges, sizeof(int));
    int *row_ptr = calloc(nodes+1, sizeof(int));

    initialize_CSR(nodes, edges, f, val, row_ptr, col_ind);
    
    float In_dregree[nodes];

    In_Degree(In_dregree, nodes, row_ptr);

    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("\n\nTime spent: %f seconds.\n", time_spent);

    //RANKING PHASE
    printf("Insert number of top k-nodes that you want to visualize :");
    scanf("%d", &n );

    float In_Degree_rank[n];
    int top_nodes[n];

    printf("In-Degree Rank:\n");
    Top_k_Rank(In_Degree_rank, top_nodes, In_dregree, nodes, n);


    return 0;
}