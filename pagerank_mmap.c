#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>




FILE *fileOpen(char *filename, char *mode){
  FILE *f;
  f = fopen(filename, mode);
  if (f == NULL){
    fprintf(stderr, "[Error] Cannot open the file");
    exit(1);
  }
  return f;
}

//READ DATASET AND RETURN NUMBER OF NODES
void readData(FILE *f, int *nodes, int *edges){
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

void initialize_CSR(FILE *f, FILE *row_pointer_file, FILE *column_index_file,float *val, int *col_index_size,int *row_pointer_size){
  int fromnode, tonode;
  int current_row = 0;
  int i = 0;
  int j = 0;
  // Elements for row
  int elem_row = 0;
  // Cumulative numbers of elements
  int current_elem = 0;
  int fdc, fdr;
  int temp_c, temp_r;

  // The first row always starts at position 0
  temp_r = 0;
  fwrite(&temp_r, sizeof(int), 1, row_pointer_file);

  while(!feof(f)){ 
    fscanf(f,"%d%d",&fromnode,&tonode);

    // CHECK IF WE NEED TO CHANGE THE ROW
    if (fromnode > current_row) {
      current_elem = current_elem + elem_row;
      for (int k = current_row + 1; k <= fromnode; k++) {
        fwrite(&current_elem, sizeof(int), 1, row_pointer_file);
      }
      elem_row = 0;
      current_row = fromnode;
    }
    val[i] = 1.0;
    fwrite(&tonode, sizeof(int), 1, column_index_file);
    elem_row++;
    i++;
  }
  temp_r = current_elem + elem_row - 1;
  fwrite(&temp_r, sizeof(int), 1, row_pointer_file);
  fclose(row_pointer_file);
  fclose(column_index_file);
  *row_pointer_size = current_row+2;
  *col_index_size = i;
  
}

void fix_stochastization(int *out_link,int nodes, float *val,int *row_ptr){
  
  int i;
  int row_elem = 0;
  int current_col = 0;
  int j;
  for(i=0; i<nodes; i++) {
    out_link[i] = 0;
  }


 
  for(i=0; i<nodes; i++) {
    if (row_ptr[i+1] != 0) {
      row_elem = row_ptr[i+1] - row_ptr[i];
      out_link[i] = row_elem;
    }
  }

  for(i=0; i<nodes; i++) {
    row_elem = row_ptr[i+1] - row_ptr[i];
    for (j=0; j<row_elem; j++) {
      val[current_col] = val[current_col] / out_link[i];
      current_col++;
    }
  }

}

int pagerank(int nodes,float *p,int *row_pointer,int *col_index, float *val){
  float d = 0.85;
  int i,j;
  int loop = 1;
  int k = 0;
  
  // Initialize p[] vector
  for(i=0; i<nodes; i++){
    p[i] = 1.0/nodes;
  }

  // Initialize new p vector
  float p_new[nodes];
  
  while (loop){
    
    // Initialize p_new as a vector of n 0.0 cells
    for(i=0; i<nodes; i++){
      p_new[i] = 0.;
    }
    
    int row_element = 0;
    int current_col = 0;
    
    // Page rank modified algorithm 
    for(i=0; i<nodes; i++){
      row_element = row_pointer[i+1] - row_pointer[i];
      for (j=0; j<row_element; j++) {
        p_new[col_index[current_col]] = p_new[col_index[current_col]] + val[current_col] * p[i];
        current_col++;
      }
    }

  
    // Adjustment to manage dangling elements 
    for(i=0; i<nodes; i++){
      p_new[i] = d * p_new[i] + (1.0 - d) / nodes;
    }

  
       
    // TERMINATION: check if we have to stop
    float error = 0.0;
    for(i=0; i<nodes; i++) {
      error =  error + fabs(p_new[i] - p[i]);
    }
    //if two consecutive instances of pagerank vector are almost identical, stop
    if (error < 0.000001){
      loop= 0;
    }
    
    // Update p[]
    for (i=0; i<nodes;i++){
        p[i] = p_new[i];
    }
    
    // Increase the number of iterations
    k = k + 1;
}
return k;


} 

int main(){
  clock_t begin, end;
  double time_spent;

  FILE *f;
  FILE *column_index_file, *row_pointer_file;

  int nodes, edges;
  int i,n;
  int col_index_size, row_pointer_size;
  int *row_pointer, *col_index;
  int file_column, file_row;
  begin = clock();


  // OPEN DATASET
  char filename[] = "web-NotreDame.txt";
  f = fileOpen(filename, "r");

  // READ DATASET
  readData(f, &nodes, &edges);
  printf("numero di nodi = %d e archi = %d \n", nodes, edges);

  float *val = calloc(edges, sizeof(float));
  int out_link[nodes];
  int n_iterations;
  float p[nodes];



  


  column_index_file = fopen("./col_ind_map", "w+");
  row_pointer_file =  fopen("./row_ptr_map", "w+");

  initialize_CSR(f,row_pointer_file,column_index_file,val,&col_index_size,&row_pointer_size);

  file_row = open("./row_ptr_map", O_RDONLY);
  row_pointer = (int *) mmap(0, row_pointer_size * sizeof(int), PROT_READ, MAP_SHARED, file_row, 0);
  if (row_pointer == MAP_FAILED) {
     close(file_row);
     printf("Error with the mapping of the file");
     exit(1);
   }
   close(file_row);

  file_column = open("./col_ind_map", O_RDONLY);
  col_index = (int *) mmap(0, col_index_size * sizeof(int), PROT_READ, MAP_SHARED, file_column, 0);
  if (col_index == MAP_FAILED) {
     close(file_column);
     printf("Error mmapping the file");
     exit(1);
   }
   close(file_column);

  fix_stochastization(out_link,nodes,val,row_pointer);

  n_iterations = pagerank(nodes,p,row_pointer,col_index,val);




  if (munmap(col_index, col_index_size * sizeof(int)) == -1) {
    printf("Error un-mmapping the file");
    exit(1);
  }

  if (munmap(row_pointer, row_pointer_size * sizeof(int)) == -1) {
    printf("Error un-mmapping the file");
    exit(1);
  }
  


  // Stop the timer and compute the time spent
  end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  printf ("Number of iteration to converge: %d \n", n_iterations); 
  printf("]\n\nTime spent: %f seconds.\n", time_spent);
  return 0;
}
