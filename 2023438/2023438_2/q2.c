#include <stdio.h>
#include <stdlib.h>

#define CODE_BASE 32768    // Base of Code Segment (32KB)
#define CODE_LIMIT 2048    // Limit of Code Segment (2KB)
#define HEAP_BASE 34816    // Base of Heap Segment (34KB)
#define HEAP_LIMIT 3072     // Limit of Heap Segment (3KB)
#define STACK_BASE 28672    // Base of Stack Segment (28KB)
#define STACK_LIMIT 2048    // Limit of Stack Segment (2KB)
#define SEGMENT_COUNT 3     // Number of segments

void AdressTranslation(unsigned int logicAddress){
    const unsigned int SEG_MASK=0xC000;  
    const unsigned int OFFSET_MASK=0x3FFF; 
    unsigned int segment=(logicAddress & SEG_MASK)>>14; 
    unsigned int offset=logicAddress & OFFSET_MASK;
    unsigned int base[SEGMENT_COUNT] ={CODE_BASE,HEAP_BASE,STACK_BASE};
    unsigned int bounds[SEGMENT_COUNT] ={CODE_LIMIT,HEAP_LIMIT,STACK_LIMIT};

    //to check for segmentation fault
    if (segment>=SEGMENT_COUNT){
        printf("Segmentation Fault\n");
        return;
    }

    if (segment==2){
        if (offset>STACK_LIMIT){
            printf("Segmentation Fault\n");
            return;
        }

        if (STACK_BASE-offset<CODE_BASE){ 
            printf("Segmentation Fault\n");
            return;
        }
    } else{ 
        if (offset>=bounds[segment]){
            printf("Segmentation Fault\n");
            return;
        }
    }
    //to get physical address
    unsigned int phyAddress;
    if (segment==2){ 
        phyAddress=STACK_BASE-offset;
    } else{
        phyAddress=base[segment]+offset;
    }
    printf("Physical Address: %04X\n",phyAddress);
}

int main(){
    char input[5]; //4 hex dig+null terminator
    printf("Enter a 16-bit logical address in hex ");
    scanf("%4s",input);
    unsigned int logicAddress=(unsigned int)strtol(input,NULL,16);
    AdressTranslation(logicAddress);

    return 0;
}
