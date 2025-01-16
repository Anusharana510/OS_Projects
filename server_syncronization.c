#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SERVERS 5
#define ROUNDS 3

sem_t ch[SERVERS]; //network channels
pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER; //output lock

void* process(void* arg){
    int id=*(int*)arg;
    int l=id, r=(id+1) % SERVERS;
    for (int count=0; count < ROUNDS; ++count){
        if (id % 2){
            sem_wait(&ch[r]);
            sem_wait(&ch[l]);
        } else{
            sem_wait(&ch[l]);
            sem_wait(&ch[r]);
        }
        pthread_mutex_lock(&mtx);
        printf("S%d processing round %d\n", id+1, count+1);
        pthread_mutex_unlock(&mtx);
        sleep(1);
        sem_post(&ch[l]);
        sem_post(&ch[r]);
        pthread_mutex_lock(&mtx);
        printf("S%d waiting\n", id+1);
        pthread_mutex_unlock(&mtx);
        sleep(1);
    }
    return NULL;
}

int main(){
    pthread_t t[SERVERS];
    int ids[SERVERS];
    for (int i=0; i < SERVERS; ++i)
        sem_init(&ch[i], 0, 1);
    for (int i=0; i < SERVERS; ++i){
        ids[i]=i;
        pthread_create(&t[i], NULL, process, &ids[i]);
    }
    for (int i=0; i < SERVERS; ++i)
        pthread_join(t[i], NULL);
    for (int i=0; i < SERVERS; ++i)
        sem_destroy(&ch[i]);
    return 0;
}
