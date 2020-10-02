#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

// CALCULATE THE NUMBER OF LINES OF THE DOCUMENT
int lines(FILE *fn){
    int sum = 0;
    char str[30];

    while(!feof(fn)){
        fgets(str, 30 - 1, fn);
        sum = sum + 1;
    }

    return (sum - 3)/3;
}

//CALCULATE JACCARD COEFFICIENT
float jaccard(int *v1, int *v2, int n){
    int i,j;
    float sum = 0.0;

    for(i=0; i<n; i++){
        for(j=0; j<n; j++){
            if(v1[i] == v2[j]){
                sum = sum + 1.0;
            }
        }
    }

    return sum / (n * 2 - sum);
}

int main(){
    int i,n,ret;
    float result1, result2, result3;
    FILE *f;

    char filename[] = "Jaccard.txt";
    f = fopen(filename, "r");

    n = lines(f);
    fclose(f);
    f = fopen(filename, "r");

    int *hits = calloc(n, sizeof(int));
    int *pagerank = calloc(n, sizeof(int));
    int *inDegree = calloc(n, sizeof(int));

    char str[100];

    while(!feof(f)){
        fgets(str, 100 - 1, f);
        if (strcmp(str,"Hits Authority top-k nodes:\n") == 0){
            for(i=0; i<n; i++){
                fgets(str, 100 - 1, f);
                hits[i] = atoi(str);
            }
        }   else if (strcmp(str,"In-Degree top-k nodes:\n") == 0){
            for(i=0; i<n; i++){
                fgets(str, 100 - 1, f);
                inDegree[i] = atoi(str);
            }
        }   else if (strcmp(str,"Pagerank top-k nodes:\n") == 0){
            for(i=0; i<n; i++){
                fgets(str, 100 - 1, f);
                pagerank[i] = atoi(str);
            }
        }
    }
    
    fclose(f);

    result1 = jaccard(hits, inDegree, n);
    result2 = jaccard(pagerank, inDegree, n);
    result3 = jaccard(hits, pagerank, n);

    printf("Jaccard result between Authority and In-Degree: %.2f\n", result1);
    printf("Jaccard result between Pagerank and In-Degree: %.2f\n", result2);
    printf("Jaccard result between Authority and Pagerank: %.2f\n", result3);

    return 0;
}