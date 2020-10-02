#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

//OPEN DATASET AND RETURN FILE
FILE *open(char *filename, char *mode){
  FILE *f;
  f = fopen(filename, mode);
  if (f == NULL){
    fprintf(stderr, "[Error] Cannot open the file");
    exit(1);
  }
  return f;
}

//READ DATASET AND RETURN NUMBER OF NODES
void read(FILE *f, int *nodes, int *edges){
  char character;
  char str[100];
  character = getc(f);
  while (character == '#'){
    fgets(str, 100 - 1, f);
    sscanf(str, "%*s %d %*s %d", nodes, edges);
    character = getc(f);
  }
  ungetc(character, f);
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
  int current_elem = 0;

  row_ptr[0] = 0;

  while (!feof(f)){
    fscanf(f, "%d%d", &fromnode, &tonode);
   
    /*
    printf("From: %d To: %d\n",fromnode, tonode);
    */

    // CHECK IF WE NEED TO CHANGE THE ROW
    if (fromnode > cur_row){ // change the row
      current_elem = current_elem + elem_row;
      for (int k = cur_row + 1; k <= fromnode; k++){
        // TELLS WHERE START THE ROW (IT'S THE ELEM+1 OF THE PREVIOUS ROW)
        row_ptr[k] = current_elem;
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
  row_ptr[cur_row + 1] = current_elem + elem_row - 1;
}

void fix_stochastization(int *out_link,int nodes,int *row_pointer, float *val){
    int i;
    int row_elem = 0;
    int current_col = 0;
    int j;
    //INITIALIZE VECTOR FOR OUTLINK TO 0
    for (i = 0; i < nodes; i++){
      out_link[i] = 0;
    }

    for (i = 0; i < nodes; i++){
      //IF NOT THE FIRST ELEMENT OF ROW_POINTER
      if (row_pointer[i + 1] != 0){
        //TELL US HOW MANY ELEMENTS THERE ARE IN THE SAME ROW OF THE MATRIX
        row_elem = row_pointer[i + 1] - row_pointer[i];
        //HOW MANY OUTLINK WE HAVE FOR THAT NODE
        out_link[i] = row_elem;
      }
    }
   
    for (i = 0; i < nodes; i++){
      //TELL US HOW MANY ELEMENTS THERE ARE IN THE SAME ROW
      row_elem = row_pointer[i + 1] - row_pointer[i];
      for (j = 0; j < row_elem; j++){
        val[current_col] = val[current_col] / out_link[i];
        current_col++;
      }
    }
  

  }

int pagerank(int nodes,int *row_ptr,int *col_ind,float *val,float *p){
    float d = 0.85;
    int i = 0;
    int j = 0;
    int loop = 1;
    int k = 0;

    float v_new[nodes];

    for(i=0; i<nodes; i++){
      p[i] = 1.0/nodes;
    }
    
  
    while (loop){
      for(i=0; i<nodes; i++){
        v_new[i] = 0.;
      }

      int row_element = 0;
      int current_col = 0;
  
      // Page rank modified algorithm 
      for(i=0; i<nodes; i++){
        row_element = row_ptr[i+1] - row_ptr[i];
        for (j=0; j<row_element; j++) {
          v_new[col_ind[current_col]] = v_new[col_ind[current_col]] + val[current_col] * p[i];
          current_col++;
        }
      }
   
      // Adjustment to manage dangling elements 
      for(i=0; i<nodes; i++){
        v_new[i] = d * v_new[i] + (1.0 - d) / nodes;
      }
        
      // TERMINATION: check if we have to stop
      float error = 0.0;
      for(i=0; i<nodes; i++) {
        error =  error + fabs(v_new[i] - p[i]);
      }
      //if two consecutive instances of pagerank vector are almost identical, stop
      if (error < 0.000001){
        loop = 0;
      }
      
      // Update p[]
      for (i=0; i<nodes;i++){
          p[i] = v_new[i];
      }
      
      // Increase the number of iterations
      k = k + 1;
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

void rank(float *r, int *t, int n, float *h, int nodes){
  int i;
  for(i=0; i<n; i++){
    r[i] = h[i];
    t[i] = i;
  }

  sort(r, t, n);

  for(i=n; i<nodes; i++){
    if (r[0] < h[i]){
      r[0] = h[i];
      t[0] = i;
      sort(r, t, n);
    }
  }

  for(i=n-1; i>=0; i--){
    printf("%d   %f\n", t[i], r[i]);
  }
}

void writeOnFile(int *t, int n){
  int i;
  FILE *f;
  char filename[] = "Jaccard.txt";
  f = open(filename, "a");

  fprintf(f, "Pagerank top-k nodes:\n");

  for(i=n-1; i>=0; i--){
    fprintf(f, "%d\n", t[i]);
  }

   fclose(f);
}
int main(){
  // Keep track of the execution time
  clock_t begin, end;
  double time_spent;
  FILE *f;
  int nodes, edges;
  int i,n;
  begin = clock();


  // OPEN DATASET
  char filename[] = "web-NotreDame.txt";
  f = open(filename, "r");

  // READ DATASET
  read(f, &nodes, &edges);
  printf("numero di nodi = %d e archi = %d \n", nodes, edges);

  /* Compressed sparse row format: 
     - Val vector: contains 1.0 if an edge exists in a certain row
     - Col_ind vector: contains the column index of the corresponding value in 'val'
     - Row_ptr vector: points to the start of each row in 'col_ind'
  */

  float *val = calloc(edges, sizeof(float));
  int *col_ind = calloc(edges, sizeof(int));
  int *row_ptr = calloc(nodes + 1, sizeof(int));

  int out_link[nodes];
  float p[nodes];
  int n_iteration;

  initialize_CSR(nodes, edges, f, val, row_ptr, col_ind);
  fix_stochastization(out_link,nodes,row_ptr,val);
  n_iteration = pagerank(nodes,row_ptr,col_ind,val,p);

  

  end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  printf ("Number of iteration to converge: %d \n", n_iteration); 

  printf("]\n\nTime spent: %f seconds.\n", time_spent);

  printf("Insert number of top k-nodes that you want to visualize :");
  scanf("%d", &n);

  float pagerank_rank[n];
  int top_nodes[n];
  printf("Page ranking:\n");
  rank(pagerank_rank, top_nodes,n, p, nodes);
  writeOnFile(top_nodes, n);
  return 0;
}