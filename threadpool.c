#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

// Matrix dimensions
int m,n,p,**A,**B,**C;
pthread_t *threads;              
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;   
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;    
int tasks_left=0,task_idx=0;

// Allocate memory for a matrix
void alloc_matrix(int ***matrix,int rows,int cols){
    *matrix=malloc(rows * sizeof(int *));
    for(int i=0; i<rows; i++)(*matrix)[i]=malloc(cols * sizeof(int));
}

// Input matrix elements
void input_matrix(int **matrix,int rows,int cols){
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++) scanf("%d",&matrix[i][j]);
}

// Compute a single element of the result matrix
void compute_element(int row,int col){
    int sum=0;
    for(int k=0; k<n; k++) sum += A[row][k] * B[k][col];
    C[row][col]=sum;
}

// Sequential multiplication
void seq_mult(){
    for(int i=0; i<m; i++)
        for(int j=0; j<p; j++) compute_element(i,j);
}

// Thread function for parallel multiplication
void *worker(void *arg){
    while(1){
        pthread_mutex_lock(&lock);
        while(tasks_left == 0 && task_idx<m * p) pthread_cond_wait(&cond,&lock);
        if(task_idx >= m * p){
            pthread_mutex_unlock(&lock);
            break;
        }
        int row=task_idx / p,col=task_idx % p;
        task_idx++;
        pthread_mutex_unlock(&lock);

        compute_element(row,col);

        pthread_mutex_lock(&lock);
        tasks_left--;
        if(tasks_left == 0) pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

// Parallel multiplication using thread pool
void parallel_mult(){
    tasks_left=m * p;
    int num_threads=get_nprocs();
    threads=malloc(num_threads * sizeof(pthread_t));
    for(int i=0; i<num_threads; i++) pthread_create(&threads[i],NULL,worker,NULL);

    pthread_mutex_lock(&lock);
    while(tasks_left > 0) pthread_cond_wait(&cond,&lock);
    pthread_mutex_unlock(&lock);

    pthread_mutex_lock(&lock);
    task_idx=m * p; 
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);

    for(int i=0; i<num_threads; i++) pthread_join(threads[i],NULL);
    free(threads);
}

// Measure execution time
double measure_time(void(*func)()){
    struct timeval start,end;
    gettimeofday(&start,NULL);
    func();
    gettimeofday(&end,NULL);
    return(end.tv_sec - start.tv_sec) * 1000.0 +(end.tv_usec - start.tv_usec) / 1000.0;
}

int main(){
    printf("Enter dimensions(m n p): ");
    scanf("%d %d %d",&m,&n,&p);

    alloc_matrix(&A,m,n);
    alloc_matrix(&B,n,p);
    alloc_matrix(&C,m,p);

    printf("Enter Matrix A elements:\n");
    input_matrix(A,m,n);
    printf("Enter Matrix B elements:\n");
    input_matrix(B,n,p);

    printf("\nSequential multiplication...\n");
    double seq_time=measure_time(seq_mult);
    printf("Time: %.2f ms\n",seq_time);

    printf("\nParallel multiplication...\n");
    double par_time=measure_time(parallel_mult);
    printf("Time: %.2f ms\n",par_time);

    printf("\nResultant Matrix C:\n");
    for(int i=0; i<m; i++){
        for(int j=0; j<p; j++) printf("%d ",C[i][j]);
        printf("\n");
    }

    printf("\nSpeed-up: %.2fx\n",seq_time / par_time);

    for(int i=0; i<m; i++) free(A[i]),free(C[i]);
    for(int i=0; i<n; i++) free(B[i]);
    free(A),free(B),free(C);

    return 0;
}
