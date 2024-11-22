#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MAX_PAGES 100

struct page {
    uint8_t page_number;
    int reference_bit;
    int dirty_bit;
};


int page_num_find(struct page *frames, int num_frames, uint8_t page_number) {

    int i = 0;
    while(i < num_frames){
        int k = frames[i].page_number;
        if (k == page_number) {
            return i;
        }
        i++;
    }
    return -1;
}

int clock_algo(struct page *frames, int num_frames, int clc_hand) {
    while (1) {
        if (frames[clc_hand].reference_bit == 1) {
            frames[clc_hand].reference_bit = 0;
            clc_hand = (clc_hand + 1) % num_frames;
        }else{
            return clc_hand;
        }
    }

    return -1;
}

void frames_output(struct page *frames, int num_frames) {
    printf("Frames: ");
    for (int i = 0; i < num_frames; i++) {
        int k = frames[i].page_number;
        if (k == UINT8_MAX) {
            printf("[ ]");
        } else {
            printf("[%d] ", k);
        }
    }
    printf("\n");
}

char* my_strdup(const char* s) {
    size_t len = strlen(s) + 1; 
    char* new_str = malloc(len);
    if (new_str == NULL) {
        return NULL; 
    }
    memcpy(new_str, s, len);
    return new_str;
}

void sample_testing(int num_frames, const char* page_requests) {
    struct page frames[MAX_PAGES];
    int clc_hand = 0;
    int fault = 0;
    int hits = 0;

    printf("Number of frames: %d\n", num_frames);
    printf("Page requests: %s\n\n", page_requests);

    int i = 0;

    while(i < num_frames){
        frames[i].page_number = UINT8_MAX;
        frames[i].reference_bit = 0;
        frames[i].dirty_bit = 0;
        i++;
    }

    char *requests_copy = my_strdup(page_requests);
    char *token = strtok(requests_copy, ",");
    if(token == NULL){
        printf("Error: strtok returned NULL\n");
        return;
    }else{
        while (token != NULL) {
        uint8_t page_number = (uint8_t)atoi(token);
        int page_index = page_num_find(frames, num_frames, page_number);

        if (page_index == -1) {
            int replace_index = clock_algo(frames, num_frames, clc_hand);
            frames[replace_index].page_number = page_number;
            frames[replace_index].reference_bit = 1;
            frames[replace_index].dirty_bit = 0;
            clc_hand = (replace_index + 1) % num_frames;
            fault++;
            printf("Page %d: Fault\n", page_number);
        } else {
            frames[page_index].reference_bit = 1;
            hits += 1;
            printf("Page %d: Hit\n", page_number);
        }

        frames_output(frames, num_frames);
        token = strtok(NULL, ",");
    }

        int total_requests = hits + fault;
        float hit_rate = (float)hits / total_requests * 100;
        printf("\nResults : \n");
        printf("Total page faults : %d\n", fault);
        printf("Total page hits : %d\n", hits);
        printf("Hit rate : %.2f%%\n\n", hit_rate);

        free(requests_copy);
    }

    
}

int main() {
    sample_testing(3, "1,2,3,4,1,2,5,1,2,3,4,5");
    sample_testing(2, "1,2,3,2,1,5,2,1,6,2,5,6,3,1,3,6,1,2,4,3");
    sample_testing(5, "1,2,3,4,5,6,7,8,9,7,2,3,4,1,6,2,7,8,9,1");
    sample_testing(4, "1,2,3,4,1,2,3,4,1,2,3,4");
    sample_testing(3, "1,2,3,4,5,6,7,8,9,10");

    return 0;
}


