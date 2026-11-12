/*
 * ALGORITHM BATTLE - Which Data Structure Wins?
 * Application : Employee ID Search
 * Structures  : BST, AVL Tree, Hash Table (Linear Probing)
 * Compile     : gcc algorithm_battle.c -o algorithm_battle
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 40
#define HSIZE 61
#define NABS 9
#define NNEW 4

typedef struct { int id; char name[16]; char dept[16]; } Employee;

/* ---------------- Dataset (40 unique employee records) ---------------- */
static const int IDS[N] = {1331,1970,1154,1404,1666,1049,1074,1840,1548,1096,
    1374,1596,1059,1931,1519,1219,1038,1088,1444,1428,1071,1246,1092,1564,1434,
    1060,1846,1579,1126,1228,1645,1642,1063,1590,1599,1406,1050,1999,1226,1047};
static const char *NAMES[N] = {"Karthik","Rahul","Divya","Suresh","Karthik","Meena",
    "Divya","Divya","Arun","Meena","Rahul","Anita","Anita","Suresh","Karthik","Divya",
    "Suresh","Anita","Anita","Divya","Rahul","Priya","Anita","Arun","Priya","Priya",
    "Anita","Anita","Divya","Anita","Arun","Anita","Rahul","Arun","Priya","Divya",
    "Arun","Suresh","Meena","Rahul"};
static const char *DEPTS[N] = {"Finance","IT","Sales","Sales","HR","Finance","Sales",
    "Sales","Sales","Ops","Finance","Sales","Finance","IT","IT","Sales","Sales",
    "Finance","Finance","HR","IT","IT","Ops","HR","Finance","Sales","Sales","HR",
    "Finance","HR","Finance","Finance","Finance","Ops","IT","Ops","IT","IT","Ops","Ops"};
static const int ABSENT[NABS]   = {999,1001,1234,1500,1777,2000,2500,500,1850};
static const int NEW_KEYS[NNEW] = {1111,1222,1333,1555};

/* ---------------- Tree node (shared by BST and AVL) ---------------- */
typedef struct Node {
    int key; Employee rec;
    struct Node *left, *right;
    int h;
} Node;

static Node *new_node(Employee r) {
    Node *n = (Node *)malloc(sizeof(Node));
    n->key = r.id; n->rec = r;
    n->left = n->right = NULL; n->h = 1;
    return n;
}

static int height_of(Node *n) { return n ? n->h : 0; }
static int max_int(int a, int b) { return a > b ? a : b; }

static int tree_height(Node *n) {            /* recomputed height (used for BST) */
    if (!n) return 0;
    return 1 + max_int(tree_height(n->left), tree_height(n->right));
}

static void free_tree(Node *n) {
    if (!n) return;
    free_tree(n->left); free_tree(n->right); free(n);
}

/* ---------------- Traversals ---------------- */
static void inorder(Node *n, int *out, int *c)   { if (n) { inorder(n->left,out,c); out[(*c)++]=n->key; inorder(n->right,out,c); } }
static void preorder(Node *n, int *out, int *c)  { if (n) { out[(*c)++]=n->key; preorder(n->left,out,c); preorder(n->right,out,c); } }
static void postorder(Node *n, int *out, int *c) { if (n) { postorder(n->left,out,c); postorder(n->right,out,c); out[(*c)++]=n->key; } }

static void print_first10(const char *tag, const char *trav, Node *root,
                          void (*fn)(Node*, int*, int*)) {
    int out[256], c = 0;
    fn(root, out, &c);
    printf("%s %-9s: [", tag, trav);
    for (int i = 0; i < 10 && i < c; i++) printf("%d%s", out[i], i < 9 ? ", " : "");
    printf("] ...\n");
}

/* ---------------- 1. Binary Search Tree ---------------- */
/* returns number of comparisons */
static int bst_insert(Node **root, Employee r) {
    Node *node = new_node(r);
    int cmp = 0;
    if (!*root) { *root = node; return 0; }
    Node *cur = *root;
    for (;;) {
        cmp++;
        if (r.id < cur->key) {
            if (!cur->left) { cur->left = node; return cmp; }
            cur = cur->left;
        } else {
            if (!cur->right) { cur->right = node; return cmp; }
            cur = cur->right;
        }
    }
}

/* shared by BST and AVL: returns found (1/0), comparisons via *cmp */
static int tree_search(Node *root, int key, int *cmp) {
    Node *cur = root;
    *cmp = 0;
    while (cur) {
        (*cmp)++;
        if (key == cur->key) return 1;
        cur = key < cur->key ? cur->left : cur->right;
    }
    return 0;
}

/* ---------------- 2. AVL Tree ---------------- */
static int avl_rotations = 0;
static int avl_cmp = 0;

static void upd(Node *n) { n->h = 1 + max_int(height_of(n->left), height_of(n->right)); }
static int balance(Node *n) { return height_of(n->left) - height_of(n->right); }

static Node *rot_right(Node *y) {
    Node *x = y->left;
    y->left = x->right; x->right = y;
    upd(y); upd(x); avl_rotations++;
    return x;
}
static Node *rot_left(Node *x) {
    Node *y = x->right;
    x->right = y->left; y->left = x;
    upd(x); upd(y); avl_rotations++;
    return y;
}

static Node *avl_ins(Node *n, Employee r) {
    if (!n) return new_node(r);
    avl_cmp++;
    if (r.id < n->key) n->left  = avl_ins(n->left, r);
    else               n->right = avl_ins(n->right, r);
    upd(n);
    int b = balance(n);
    if (b > 1 && r.id < n->left->key)   return rot_right(n);            /* LL */
    if (b < -1 && r.id >= n->right->key) return rot_left(n);            /* RR */
    if (b > 1)  { n->left  = rot_left(n->left);   return rot_right(n); } /* LR */
    if (b < -1) { n->right = rot_right(n->right); return rot_left(n);  } /* RL */
    return n;
}

static int avl_insert(Node **root, Employee r) {
    avl_cmp = 0;
    *root = avl_ins(*root, r);
    return avl_cmp;
}

/* ---------------- 3. Hash Table (Linear Probing) ---------------- */
typedef struct {
    Employee table[HSIZE];
    int used[HSIZE];
    int collisions;
} HashTable;

static void ht_init(HashTable *h) { memset(h, 0, sizeof(*h)); }

/* returns probes; each occupied slot hit counts as one collision */
static int ht_insert(HashTable *h, Employee r) {
    int i = r.id % HSIZE, probes = 1;
    while (h->used[i]) {
        h->collisions++;
        i = (i + 1) % HSIZE;
        probes++;
    }
    h->table[i] = r; h->used[i] = 1;
    return probes;
}

static int ht_search(HashTable *h, int key, int *probes) {
    int i = key % HSIZE;
    *probes = 0;
    for (int k = 0; k < HSIZE; k++) {
        (*probes)++;
        if (!h->used[i]) return 0;
        if (h->table[i].id == key) return 1;
        i = (i + 1) % HSIZE;
    }
    return 0;
}

/* ---------------- Experiment helpers ---------------- */
enum { T_BST, T_AVL, T_HASH };

static int do_search(int type, Node *bst, Node *avl, HashTable *h, int key, int *c) {
    if (type == T_BST)  return tree_search(bst, key, c);
    if (type == T_AVL)  return tree_search(avl, key, c);
    return ht_search(h, key, c);
}

static void search_stats(const char *label, const int *keys, int n,
                         Node *bst, Node *avl, HashTable *h) {
    const char *names[3] = {"BST ", "AVL ", "Hash"};
    printf("%s  total / avg / max comparisons\n", label);
    for (int t = 0; t < 3; t++) {
        int total = 0, mx = 0, c;
        for (int i = 0; i < n; i++) {
            do_search(t, bst, avl, h, keys[i], &c);
            total += c; if (c > mx) mx = c;
        }
        printf("  %s: %4d / %5.2f / %d\n", names[t], total, (double)total / n, mx);
    }
}

static int cmp_emp(const void *a, const void *b) {
    return ((const Employee *)a)->id - ((const Employee *)b)->id;
}

static void run(Employee *recs, const char *label, int show_traversal) {
    Node *bst = NULL, *avl = NULL;
    HashTable ht;
    ht_init(&ht);
    avl_rotations = 0;
    int ins_bst = 0, ins_avl = 0, ins_hash = 0;

    for (int i = 0; i < N; i++) {
        ins_bst  += bst_insert(&bst, recs[i]);
        ins_avl  += avl_insert(&avl, recs[i]);
        ins_hash += ht_insert(&ht, recs[i]);
    }

    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = recs[i].id;

    printf("\n================================================================\n");
    printf("%s  (%d records)\n", label, N);
    printf("================================================================\n");

    if (show_traversal) {
        print_first10("BST", "Inorder",   bst, inorder);
        print_first10("BST", "Preorder",  bst, preorder);
        print_first10("BST", "Postorder", bst, postorder);
        print_first10("AVL", "Inorder",   avl, inorder);
        print_first10("AVL", "Preorder",  avl, preorder);
        print_first10("AVL", "Postorder", avl, postorder);
        printf("(Inorder of BST/AVL is sorted -> sorted employee listing)\n");
    }

    printf("\nHeight  : BST = %d | AVL = %d | AVL rotations = %d\n",
           tree_height(bst), height_of(avl), avl_rotations);
    printf("Collisions (Hash, size %d) : %d | load factor = %.2f\n",
           HSIZE, ht.collisions, (double)N / HSIZE);
    printf("Insertion comparisons/probes (total for build): BST=%d, AVL=%d, Hash=%d\n",
           ins_bst, ins_avl, ins_hash);

    printf("\n");
    search_stats("SUCCESSFUL SEARCH (all keys)", keys, N, bst, avl, &ht);
    printf("\n");
    search_stats("UNSUCCESSFUL SEARCH (9 absent keys)", ABSENT, NABS, bst, avl, &ht);

    printf("\nINSERTING NEW KEYS [1111, 1222, 1333, 1555]  (comparisons/probes each)\n");
    printf("  BST : [");
    for (int i = 0; i < NNEW; i++) { Employee e = {NEW_KEYS[i], "New", "IT"}; printf("%d%s", bst_insert(&bst, e), i < NNEW-1 ? ", " : ""); }
    printf("]\n  AVL : [");
    for (int i = 0; i < NNEW; i++) { Employee e = {NEW_KEYS[i], "New", "IT"}; printf("%d%s", avl_insert(&avl, e), i < NNEW-1 ? ", " : ""); }
    printf("]\n  Hash: [");
    for (int i = 0; i < NNEW; i++) { Employee e = {NEW_KEYS[i], "New", "IT"}; printf("%d%s", ht_insert(&ht, e), i < NNEW-1 ? ", " : ""); }
    printf("]\n  Height after inserts: BST = %d | AVL = %d\n", tree_height(bst), height_of(avl));

    /* ---- Viva demo (only after experiment 1) ---- */
    if (show_traversal) {
        int key = 1234, c;
        printf("\nVIVA DEMO - search key %d\n", key);
        const char *nm[3] = {"BST ", "AVL ", "Hash"};
        for (int t = 0; t < 3; t++) {
            int f = do_search(t, bst, avl, &ht, key, &c);
            printf("  %s: %s after %d comparisons\n", nm[t], f ? "FOUND" : "NOT FOUND", c);
        }
    }

    free_tree(bst);
    free_tree(avl);
}

int main(void) {
    Employee recs[N];
    for (int i = 0; i < N; i++) {
        recs[i].id = IDS[i];
        strcpy(recs[i].name, NAMES[i]);
        strcpy(recs[i].dept, DEPTS[i]);
    }
    printf("Dataset (first 3): (%d,%s,%s) (%d,%s,%s) (%d,%s,%s)\n",
           recs[0].id, recs[0].name, recs[0].dept,
           recs[1].id, recs[1].name, recs[1].dept,
           recs[2].id, recs[2].name, recs[2].dept);

    run(recs, "EXP 1: Random insertion order", 1);

    qsort(recs, N, sizeof(Employee), cmp_emp);
    run(recs, "EXP 2: Sorted insertion order (worst case for BST)", 0);
    return 0;
}
