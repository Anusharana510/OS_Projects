#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SIZE 10  //buffer storage
#define MAX_PROD 5  //Max products to handle
typedef struct{
    int buf[SIZE];
    int in,out,count;
} Buffer; //circular buffer
Buffer warehouse={.in=0,.out=0,.count=0};
sem_t empty,full;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

void* truck(void* arg){
    int id=*(int*)arg;
    while (1){
        int prod=rand() % MAX_PROD + 1;
        sem_wait(&empty);
        pthread_mutex_lock(&lock);
        for (int i=0; i < prod && warehouse.count < SIZE; i++){
            warehouse.buf[warehouse.in]=1;
            warehouse.in=(warehouse.in + 1) % SIZE;
            warehouse.count++;
        }
        printf("Truck %d delivered %d items. Inventory: %d\n",id,prod,warehouse.count);
        pthread_mutex_unlock(&lock);
        sem_post(&full);
        sleep(rand() % 3 + 1);
    }
}
void* manager(void* arg){
    int id=*(int*)arg;
    while (1){
        int prod=rand() % MAX_PROD + 1;
        sem_wait(&full);
        pthread_mutex_lock(&lock);
        for (int i=0; i < prod && warehouse.count > 0; i++){
            warehouse.buf[warehouse.out]=0;
            warehouse.out=(warehouse.out + 1) % SIZE;
            warehouse.count--;
        }
        printf("Manager %d stored %d items. Inventory: %d\n",id,prod,warehouse.count);
        pthread_mutex_unlock(&lock);
        sem_post(&empty);
        sleep(rand() % 3 + 1);
    }
}
int main(){
    int trucks,managers;
    printf("Number of trucks: ");
    scanf("%d",&trucks);
    printf("Number of managers: ");
    scanf("%d",&managers);
    pthread_t t_threads[trucks],m_threads[managers];
    int ids[trucks > managers ? trucks : managers];
    sem_init(&empty,0,SIZE);
    sem_init(&full,0,0);
    for (int i=0; i < trucks; i++){
        ids[i]=i + 1;
        pthread_create(&t_threads[i],NULL,truck,&ids[i]);
    }
    for (int i=0; i < managers; i++){
        ids[i]=i + 1;
        pthread_create(&m_threads[i],NULL,manager,&ids[i]);
    }
    sleep(15);
    for (int i=0; i < trucks; i++)
        pthread_cancel(t_threads[i]);
    for (int i=0; i < managers; i++)
        pthread_cancel(m_threads[i]);
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&lock);
    printf("Final inventory: %d\n",warehouse.count);
    return 0;
}

