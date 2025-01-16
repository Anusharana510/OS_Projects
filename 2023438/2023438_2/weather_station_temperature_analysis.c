#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define ARRAY_SIZE 16  //predefined 

//to merge two sorted subarrays
void merge(int *arr,int l,int m,int h){
    int n1=m-l+1; 
    int n2=h-m;    
    //creating temp arrays for storing subarrays
    int LeftAr[n1],RigtAr[n2];
    for (int i=0;i<n1;i++)
        LeftAr[i]=arr[l+i];
    for (int i=0;i<n2;i++)
        RigtAr[i]=arr[m+1+i];
    //merging the two temp arrays back into the main array
    int i=0,j=0,k=l; 
    while (i<n1 && j<n2){
        if (LeftAr[i]<=RigtAr[j]){
            arr[k]=LeftAr[i];
            i++;
        } else{
            arr[k]=RigtAr[j];
            j++;
        }
        k++;
    }
    while (i<n1){
        arr[k]=LeftAr[i];
        i++;
        k++;
    }
    while (j<n2){
        arr[k]=RigtAr[j];
        j++;
        k++;
    }
}


//merge sort function applied recursively (as asked in the ques to break the array /2 recursively)
void mergesort(int *arr,int l,int h){
    if (l<h){
        int m=l+(h-l)/2;
        //sorting the halves
        mergesort(arr,l,m);
        mergesort(arr,m+1,h);
        //final : merging the two sorted halves
        merge(arr,l,m,h);
    }
}


//pipe functions 
//to read and write through pipe (different funxtions as half duplex communicationusing pipe)
void pipeREAD(int pipefile,int *arr,int size){
    read(pipefile,arr,size * sizeof(int));
}
void pipeWRITE(int pipefile,int *arr,int size){
    write(pipefile,arr,size * sizeof(int));
}


//applying parallel merge sort using pipe
void parallel_mergesort(int *arr,int l,int h){
    if (h-l+1<=1){ //single element ....so already sorted 
        return;
    }
     int m=l+(h-l)/2;
    int LEFTpipe[2],RIGHTpipe[2];
    //piping l and h halves
    pipe(LEFTpipe);
    pipe(RIGHTpipe);
    pid_t l_pid = fork();
    if (l_pid==0){ //l child
        close(LEFTpipe[0]); 
        parallel_mergesort(arr,l,m);
        pipeWRITE(LEFTpipe[1],arr+l,m-l+1);
        close(LEFTpipe[1]); 
        exit(0);
    }

    pid_t h_pid = fork();
    if (h_pid==0){ //h child
        close(RIGHTpipe[0]); 
        parallel_mergesort(arr,m+1,h);
        pipeWRITE(RIGHTpipe[1],arr+m+1,h-m);
        close(RIGHTpipe[1]); 
        exit(0);
    }
    //waitinf for children to finish
    waitpid(l_pid,NULL,0); 
    waitpid(h_pid,NULL,0); 
    //Reading the halves from the pipe nd then closing them
    close(LEFTpipe[1]); 
    pipeREAD(LEFTpipe[0],arr+l,m-l+1); 
    close(LEFTpipe[0]); 
    close(RIGHTpipe[1]); 
    pipeREAD(RIGHTpipe[0],arr+m+1,h-m);
    close(RIGHTpipe[0]); 
    //merging the halves
    merge(arr,l,m,h);
}

int main(){
    //hardcoding the unsorted array nd then sorting nd printinf the sorted arr
    int arr[ARRAY_SIZE]={38,27,43,3,9,82,10,23,15,28,19,41,11,7,25,45};
    printf("Initial unsorted array: \n");
    for (int i=0;i<ARRAY_SIZE;i++){
        printf("%d ",arr[i]);
    }
    printf("\n");
    parallel_mergesort(arr,0,ARRAY_SIZE-1);
    printf("Final sorted array: \n");
    for (int i=0;i<ARRAY_SIZE;i++){
        printf("%d ",arr[i]);
    }
    printf("\n");
    return 0;
}
