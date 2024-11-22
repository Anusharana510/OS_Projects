#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PSIZE 4096
#define MSIZE 65536
#define TSIZE 4
#define NUM_PAGES (MSIZE / PSIZE)

typedef struct {
    bool valid;
    unsigned int PFN;
    unsigned int protectBits;
} PTE;

typedef struct {
    unsigned int VPN;
    unsigned int PFN;
    unsigned int protectBits;
} TLBEntry;

TLBEntry TLB[TSIZE];
PTE pageTable[NUM_PAGES];

int TLB_index = 0;

bool TLB_Lookup(unsigned int VPN, TLBEntry *entry) {
    for (int i = 0; i < TSIZE; i++) {
        if (TLB[i].VPN == VPN) {
            *entry = TLB[i];
            return true;
        }
    }
    return false;
}

void TLB_insertion(unsigned int VPN, unsigned int PFN, unsigned int protectBits) {
    TLB[TLB_index].VPN = VPN;
    TLB[TLB_index].PFN = PFN;
    TLB[TLB_index].protectBits = protectBits;
    TLB_index = (TLB_index + 1) % TSIZE;
}

unsigned int AccessMemory(unsigned int physAddr) {
    return physAddr;
}

bool CanAccess(unsigned int protectBits) {
    return true;
}

void InitializePageTable() {
    for (unsigned int i = 0; i < NUM_PAGES; i++) {
        pageTable[i].valid = true;
        pageTable[i].PFN = i;
        pageTable[i].protectBits = 0;
    }
}

int main() {
    unsigned int logicalAddr;
    unsigned int VPN, offset, physAddr;
    TLBEntry entry;

    InitializePageTable();

    while (true) {
        printf("Enter a 16-bit logical address in hex (e.g., a56f) or 'exit' to quit: ");
        char input[10];
        scanf("%s", input);

        if (strcmp(input, "exit") == 0) {
            break;
        }

        sscanf(input, "%x", &logicalAddr);

        VPN = (logicalAddr / PSIZE);
        offset = logicalAddr % PSIZE;

        if (TLB_Lookup(VPN, &entry)) {
            if (CanAccess(entry.protectBits)) {
                physAddr = (entry.PFN * PSIZE) + offset;
                printf("TLB Hit\n");
                printf("Physical Address: 0x%04x\n", physAddr);
            } else {
                printf("Raise Exception: PROTECTION_FAULT\n");
            }
        } else {
            PTE *PTE = &pageTable[VPN];

            if (!PTE->valid) {
                printf("Raise Exception: SEGMENTATION_FAULT\n");
            } else if (!CanAccess(PTE->protectBits)) {
                printf("Raise Exception: PROTECTION_FAULT\n");
            } else {
                TLB_insertion(VPN, PTE->PFN, PTE->protectBits);
                physAddr = (PTE->PFN * PSIZE) + offset;
                printf("TLB Miss\n");
                printf("Physical Address: 0x%04x, TLB Entry Added\n", physAddr);
            }
        }
        printf("\n");
    }

    return 0;
}
