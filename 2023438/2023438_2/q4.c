#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define SIZE 4096
#define ENTRIES 1024
#define DIRECTORY 1024


#define R "\033[0;31m"
#define G "\033[0;32m"
#define B "\033[0;34m"
#define RE "\033[0m"

typedef struct {
    uint32_t entries[ENTRIES];
} PageTable;

typedef struct {
    PageTable* entries[DIRECTORY];
} PageDirectory;

PageDirectory page_directory = {0};
int page_hits = 0;
int page_misses = 0;

uint32_t translate_address(uint32_t va, bool is_store) {
    int Null = 0xFFFFFFFF;
    int x = 0x3FF;
    int y = 0xFFF;
    uint32_t pd_index = (va >> 22) & x;
    uint32_t pt_index = (va >> 12) & x;
    uint32_t offset = va & y;

    PageTable* pt = page_directory.entries[pd_index];
    if(pt == NULL){
        pt = (PageTable*)malloc(sizeof(PageTable));
        int i = 0;
        while(i < ENTRIES){
            if(pt->entries[i] == Null){
                pt->entries[i] = Null;
                i++;
            }
        }
    }


    page_directory.entries[pd_index] = pt;
    page_misses++;
    printf(R "Page fault: Created new page table for Page Directory index %u\n" RE "\n", pd_index);
    

    uint32_t pfn = pt->entries[pt_index];
    if (pfn == Null) {
        pfn = rand() % ENTRIES;
        pt->entries[pt_index] = pfn;
        page_misses++;
        printf(B "Page fault: Mapped Virtual Address 0x%08X to new Physical Address %u\n" RE "\n", va, pfn);
    } else {
        page_hits++;
    }

    return (pfn * 4096) | offset;
}

uint8_t load(uint32_t va) {
    uint32_t pa = translate_address(va, false);
    if(pa == -1){
        printf(R "Unable to Create Page Table.\n" RE "\n");
        pa = translate_address(va, false);
    }
    printf(G "Load from Virtual Address 0x%08X -> Physical Address 0x%08X\n" RE "\n", va, pa);
    return 0; 
}

void store(uint32_t va) {
    uint32_t pa = translate_address(va, true);
    if(pa == -1){
        printf(R "Unable to Create Page Directory." RE "\n");
        pa = translate_address(va, false);
    }

    printf(G "Storing Virtual Address to 0x%08X -> Physical Address 0x%08X\n" RE "\n", va, pa);
}

void print_summary() {
    printf("Page Directory Size: %lu bytes\n", sizeof(page_directory));
    printf("Page Table Size: %lu bytes\n", sizeof(PageTable));
    printf("Page hits: %d\n", page_hits);
    printf("Page misses: %d\n", page_misses);
    printf("Hit/Miss ratio: %.2f\n", (float)page_hits / (page_hits + page_misses));
}

int main() {


    uint32_t va1 = 0xCCC0FFEE;
    uint32_t va2 = 0xAAA0BBCC;
    uint32_t va3 = 0x12345678; 
    uint32_t va4 = 0xCCC0FFEE;
    uint32_t va5 = 0xAAA0BBCC;  
    uint32_t va6 = 0x98765421;
    uint32_t va7 = 0x11223344;
    uint32_t va8 = 0x87654321;

    
    store(va1);
    load(va1);
    store(va2);
    load(va2);
    store(va3);
    load(va3);
    store(va4);
    load(va4);
    store(va5);
    load(va5);
    store(va6);
    load(va6);
    store(va7);
    load(va7);
    store(va8);
    load(va8);

    print_summary();

    return 0;
}