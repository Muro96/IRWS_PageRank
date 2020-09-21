#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#define d 0.85


//OPEN DATASET AND RETURN FILE
FILE *open(char *filename,char *mode){
	FILE *f;
	f = fopen(filename,mode);
	if(f == NULL){
		fprintf(stderr,"[Error] Cannot open the file");
        exit(1);
	}
	return f;
}


//READ DATASET AND RETURN NUMBER OF NODES
void read(FILE *f,int *nodes, int *edges){
    char ch;
    char str[100];
    ch = getc(f);
    while (ch =='#') {
        fgets(str,100-1,f);
       	sscanf (str,"%*s %d %*s %d", nodes, edges);
        ch = getc(f);
    }
    ungetc(ch, f);
	*nodes = *nodes;
	*edges = *edges;

}

void initialize_CSR(int nodes,int edges,FILE *f, float *val,int *row_ptr,int *col_ind){
	int fromnode, tonode;
	int cur_row = 0;
	int i = 0;
	int j = 0;
	// Elements for row
	int elem_row = 0;
	// Cumulative numbers of elements
	int curel = 0;

	row_ptr[0] = 0;
  
	
	while(!feof(f)){
		fscanf(f,"%d%d",&fromnode,&tonode);
		// DEBUG: print fromnode and tonode
		//printf("From: %d To: %d\n",fromnode, tonode);

		// CHECK IF WE NEED TO CHANGE THE ROW
		if (fromnode > cur_row) { // change the row
      curel = curel + elem_row;
      for (int k = cur_row + 1; k <= fromnode; k++) {
			  // TELLS WHERE START THE ROW (IT'S THE ELEM+1 OF THE PREVIOUS ROW)
       	row_ptr[k] = curel;
        }
			
			elem_row = 0;
      //SET THE CURRENT_ROW WITH THE VALUE OF fromnode
			cur_row = fromnode;
    	}
		val[i] = 1.0;
    //
		col_ind[i] = tonode;
		elem_row++;
		i++;
  }
  row_ptr[cur_row+1] = curel + elem_row - 1;
}






int main(){
    // Keep track of the execution time
	clock_t begin, end;
	double time_spent;
	FILE *f;
	int nodes,edges;
  int i;
	begin = clock();

	// Set the damping factor 'd'
	//float d = 0.85;


	// OPEN DATASET
    char filename[] = "web-NotreDame.txt";
	f = open(filename,"r");

	// READ DATASET
	read(f,&nodes,&edges);
	printf("numero di nodi = %d e archi = %d",nodes,edges);

  /* Compressed sparse row format: 
     - Val vector: contains 1.0 if an edge exists in a certain row
     - Col_ind vector: contains the column index of the corresponding value in 'val'
     - Row_ptr vector: points to the start of each row in 'col_ind'
  */


	float *val = calloc(edges, sizeof(float));
	int *col_ind = calloc(edges, sizeof(int));
	int *row_ptr = calloc(nodes+1, sizeof(int)); 

	initialize_CSR(nodes,edges,f,val,row_ptr,col_ind);




  // Fix the stochastization
  /*int out_link[nodes];

  for(i=0; i<nodes; i++){
    out_link[i] =0;
  } */

  /* DEBUG: row pointer test
    printf("\nRow_ptr:\n");
     for (i=0; i<n; i++){
          printf("%d ", row_ptr[i]);
        }
    printf("\n");
  

  int rowel = 0;
  for(i=0; i<n; i++){
        if (row_ptr[i+1] != 0) {
          rowel = row_ptr[i+1] - row_ptr[i];
          out_link[i] = rowel;
        }
   }

  /* DEBUG: Outlink print test
  printf("\nOutlink:\n");
   for (i=0; i<n; i++){
        printf("%d ", out_link[i]);
      }
  printf("\n");
  
    
  int curcol = 0;
  for(i=0; i<n; i++){
    rowel = row_ptr[i+1] - row_ptr[i];
    for (j=0; j<rowel; j++) {
      val[curcol] = val[curcol] / out_link[i];
      curcol++;
    }
  }

  /* DEBUG: val print test 
  for(i=0; i<e; i++){
      printf("%f ", val[i]);
  }
 
  /******************* INITIALIZATION OF P, DAMPING FACTOR ************************/

  // Set the damping factor 'd'
 /*  float d = 0.85;
  
  // Initialize p[] vector
  float p[n];
  for(i=0; i<n; i++){
    p[i] = 1.0/n;
  }
  
  /*************************** PageRank LOOP  **************************/

  // Set the looping condition and the number of iterations 'k'
  /*int looping = 1;
  int k = 0;
  
  // Initialize new p vector
  float p_new[n];
  
  while (looping){
    
    // Initialize p_new as a vector of n 0.0 cells
    for(i=0; i<n; i++){
      p_new[i] = 0.0;
    }
    
    int rowel = 0;
    int curcol = 0;
    
    // Page rank modified algorithm 
    for(i=0; i<n; i++){
      rowel = row_ptr[i+1] - row_ptr[i];
      for (j=0; j<rowel; j++) {
        p_new[col_ind[curcol]] = p_new[col_ind[curcol]] + val[curcol] * p[i];
        curcol++;
      }
    }

    /*DEBUG: print pnew
    for (i=0; i<n; i++){
      printf("%f ", p_new[i]);
    }

    // Adjustment to manage dangling elements 
    for(i=0; i<n; i++){
      p_new[i] = d * p_new[i] + (1.0 - d) / n;
    }

    /*DEBUG: print pnew after the damping factor multiplication
    for (i=0; i<n; i++){
      printf("%f ", p_new[i]);
    }
       
    // TERMINATION: check if we have to stop
    float error = 0.0;
    for(i=0; i<n; i++) {
      error =  error + fabs(p_new[i] - p[i]);
    }
    //if two consecutive instances of pagerank vector are almost identical, stop
    if (error < 0.000001){
      looping = 0;
    }
    
    // Update p[]
    for (i=0; i<n;i++){
        p[i] = p_new[i];
    }
    
    // Increase the number of iterations
    k = k + 1;
}
  
/*************************** CONCLUSIONS *******************************/

  // Stop the timer and compute the time spent
  /*end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  // Sleep a bit so stdout is not messed up
  //Sleep(500);
    
  // Print results
  printf ("\nNumber of iteration to converge: %d \n\n", k); 
  printf ("Final Pagerank values:\n\n[");
  for (i=0; i<n; i++){
    printf("%f ", p[i]);
    if(i!=(n-1)){ printf(", "); }
  }
  printf("]\n\nTime spent: %f seconds.\n", time_spent); */


	

   




  return 0;
}