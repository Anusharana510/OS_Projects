#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Define the locks
pthread_mutex_t mutexA; 
pthread_mutex_t mutexB; 
//fxn for locks order
void *threadF(void *arg){
    char *threadName=(char *)arg;
    for (int i=0; i < 3; i++){ 
        printf("%s is trying to acquire Lock A\n",threadName);
        pthread_mutex_lock(&mutexA);  // Acquire Lock A
        printf("%s acquired Lock A\n",threadName);
        printf("%s is trying to acquire Lock B\n",threadName);
        pthread_mutex_lock(&mutexB);  // Acquire Lock B
        printf("%s acquired Lock B\n",threadName);
        printf("%s is working\n",threadName);
        sleep(1); // Simulate a delay for the work being done
        //locks reversed
        pthread_mutex_unlock(&mutexB); 
        printf("%s released Lock B\n",threadName);
        pthread_mutex_unlock(&mutexA);
        printf("%s released Lock A\n",threadName);
        sleep(1); //delay caused
    }
    pthread_exit(NULL); // Exit the thread
}
int main(){
    pthread_mutex_init(&mutexA,NULL);
    pthread_mutex_init(&mutexB,NULL); 
    pthread_t thread1,thread2,thread3; // thread IDs
    char *threadName1="T1";
    char *threadName2="T2";
    char *threadName3="T3";
    pthread_create(&thread1,NULL,threadF,threadName1); 
    pthread_create(&thread2,NULL,threadF,threadName2); 
    pthread_create(&thread3,NULL,threadF,threadName3); 
    // Wait for threads to finish execution
    pthread_join(thread1,NULL); 
    pthread_join(thread2,NULL); 
    pthread_join(thread3,NULL); 
    pthread_mutex_destroy(&mutexA);
    pthread_mutex_destroy(&mutexB);
    printf("All threads have completed their work.\n");
    return 0;
}