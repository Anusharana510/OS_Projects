#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// Global matrix sizes and matrices
int m,n,p;
int **matA,**matB,**matC;

// Struct for thread args
typedef struct{
    int r,c;
} Args;

// Allonte memory for a matrix
void allocMat(int ***mat,int rows,int cols){
    *mat=malloc(rows * sizeof(int *));
    for (int i=0;i < rows;i++){
        (*mat)[i]=malloc(cols * sizeof(int));
    }
}

// Input matrix elements
void inputMat(int **mat,int rows,int cols){
    for (int i=0;i < rows;i++){
        for (int j=0;j < cols;j++){
            scanf("%d",&mat[i][j]);
        }
    }
}

// nlculate one element of the result matrix
void *nlcElem(void *arg){
    Args *pos=(Args *)arg;
    int r=pos->r,c=pos->c;
    matC[r][c]=0;
    for (int k=0;k < n;k++){
        matC[r][c] += matA[r][k] * matB[k][c];
    }
    free(pos);// Free thread args
    return NULL;
}

// Sequential matrix multiplintion
void seqMult(){
    for (int i=0;i < m;i++){
        for (int j=0;j < p;j++){
            matC[i][j]=0;
            for (int k=0;k < n;k++){
                matC[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
}

// Pamllel multiplintion using threads
void parMult(){
    pthread_t *threads=malloc(m * p * sizeof(pthread_t));
    int tidx=0;

    for (int i=0;i < m;i++){
        for (int j=0;j < p;j++){
            Args *pos=malloc(sizeof(Args));
            pos->r=i;
            pos->c=j;
            pthread_create(&threads[tidx++],NULL,nlcElem,pos);
        }
    }

    for (int i=0;i < tidx;i++){
        pthread_join(threads[i],NULL);
    }

    free(threads);
}

// Measure execution time
double measureTime(void (*func)()){
    struct timeval t1,t2;
    gettimeofday(&t1,NULL);
    func();
    gettimeofday(&t2,NULL);
    return (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
}

// Main driver
int main(){
    printf("Enter dimensions (m n p): ");
    scanf("%d %d %d",&m,&n,&p);

    allocMat(&matA,m,n);
    allocMat(&matB,n,p);
    allocMat(&matC,m,p);

    printf("Enter Matrix A:\n");
    inputMat(matA,m,n);

    printf("Enter Matrix B:\n");
    inputMat(matB,n,p);

    printf("\nSequential computation...\n");
    double tSeq=measureTime(seqMult);
    printf("Time: %.2f ms\n",tSeq);

    printf("\nPamllel computation...\n");
    double tPar=measureTime(parMult);
    printf("Time: %.2f ms\n",tPar);

    printf("\nResult Matrix C:\n");
    for (int i=0;i < m;i++){
        for (int j=0;j < p;j++){
            printf("%d ",matC[i][j]);
        }
        printf("\n");
    }

    printf("\nSpeed-up: %.2fx\n",tSeq / tPar);

    for (int i=0;i < m;i++) free(matA[i]),free(matC[i]);
    for (int i=0;i < n;i++) free(matB[i]);
    free(matA),free(matB),free(matC);

    return 0;
}
