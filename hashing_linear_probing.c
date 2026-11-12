/*
 * Hashing with Linear Probing - Employee ID Search
 * Compile : gcc hashing_linear_probing.c -o hashing
 */
#include <stdio.h>
#include <string.h>

#define SIZE 61            /* table size (prime) */

typedef struct {
    int  id;
    char name[20];
    char dept[20];
    int  used;             /* 0 = empty slot, 1 = occupied */
} Employee;

Employee table[SIZE];
int collisions = 0;
int count = 0;

int hash(int key) { return key % SIZE; }

/* Insert with linear probing. Returns 1 on success, 0 if duplicate/full */
int insert(int id, const char *name, const char *dept) {
    if (count == SIZE) { printf("Table is full!\n"); return 0; }
    int i = hash(id), probes = 1;
    while (table[i].used) {
        if (table[i].id == id) { printf("ID %d already exists.\n", id); return 0; }
        collisions++;                       /* occupied slot -> collision */
        i = (i + 1) % SIZE;                 /* linear probe: next slot   */
        probes++;
    }
    table[i].id = id;
    strcpy(table[i].name, name);
    strcpy(table[i].dept, dept);
    table[i].used = 1;
    count++;
    printf("Inserted ID %d at index %d (probes = %d)\n", id, i, probes);
    return 1;
}

/* Search with linear probing. Returns index or -1; comparisons via *cmp */
int search(int id, int *cmp) {
    int i = hash(id);
    *cmp = 0;
    for (int k = 0; k < SIZE; k++) {
        (*cmp)++;
        if (!table[i].used) return -1;      /* empty slot -> not present */
        if (table[i].id == id) return i;    /* found                     */
        i = (i + 1) % SIZE;
    }
    return -1;
}

void display(void) {
    printf("\nIndex | ID   | Name     | Dept\n");
    printf("------------------------------------\n");
    for (int i = 0; i < SIZE; i++)
        if (table[i].used)
            printf("%5d | %4d | %-8s | %s\n", i, table[i].id, table[i].name, table[i].dept);
    printf("Records = %d, Collisions = %d, Load factor = %.2f\n",
           count, collisions, (double)count / SIZE);
}

int main(void) {
    int ch, id, cmp, idx;
    char name[20], dept[20];

    do {
        printf("\n--- Employee Hash Table (Linear Probing) ---\n");
        printf("1. Insert\n2. Search\n3. Display\n4. Exit\nChoice: ");
        if (scanf("%d", &ch) != 1) break;
        switch (ch) {
        case 1:
            printf("Enter ID, Name, Dept: ");
            scanf("%d %19s %19s", &id, name, dept);
            insert(id, name, dept);
            break;
        case 2:
            printf("Enter ID to search: ");
            scanf("%d", &id);
            idx = search(id, &cmp);
            if (idx >= 0)
                printf("FOUND at index %d: %d %s %s (comparisons = %d)\n",
                       idx, table[idx].id, table[idx].name, table[idx].dept, cmp);
            else
                printf("NOT FOUND (comparisons = %d)\n", cmp);
            break;
        case 3:
            display();
            break;
        }
    } while (ch != 4);
    return 0;
}
