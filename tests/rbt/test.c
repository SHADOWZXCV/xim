#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "types.h"
#include "structures/vector.h"
#include "structures/rbt.h"

// ---------------------------------------------------------
// Compare function for ints
// ---------------------------------------------------------
static short int_compare(void *a, void *b) {
    int ia = *(int *)a;
    int ib = *(int *)b;
    if (ia < ib) return -1;
    if (ia > ib) return 1;
    return 0;
}

// ---------------------------------------------------------
// Invariant checking helpers
// ---------------------------------------------------------

static void check_no_red_red(RedBlackTreeNode *node) {
    if (!node) return;

    if (node->color == RBT_COLOR_RED) {
        if (node->left) {
            assert(node->left->color == RBT_COLOR_BLACK &&
                   "Red node has red left child");
        }
        if (node->right) {
            assert(node->right->color == RBT_COLOR_BLACK &&
                   "Red node has red right child");
        }
    }

    check_no_red_red(node->left);
    check_no_red_red(node->right);
}

// Debug black-height checker with path reporting
static int black_height_debug(RedBlackTreeNode *node, int current_black,
                              int *expected, int depth) {
    if (!node) {
        // NULL leaves counted as black
        current_black++;
        if (*expected == -1) {
            *expected = current_black;
        } else if (current_black != *expected) {
            printf("BLACK-HEIGHT MISMATCH at leaf: got %d, expected %d\n",
                   current_black, *expected);
            return 0;
        }
        return 1;
    }

    if (node->color == RBT_COLOR_BLACK) {
        current_black++;
    }

    if (!black_height_debug(node->left, current_black, expected, depth + 1))
        return 0;
    if (!black_height_debug(node->right, current_black, expected, depth + 1))
        return 0;

    return 1;
}

static int check_black_height(RedBlackTreeNode *root) {
    int expected = -1;
    if (!black_height_debug(root, 0, &expected, 0)) {
        return -1;
    }
    return expected;
}

static void inorder_collect(RedBlackTree *tree, Vector *vec) {
    if (tree->root) {
        iterate_redblack_tree(tree, tree->root, vec);
    }
}

static int vector_is_sorted_int(Vector *vec) {
    if (vec->len == 0) return 1;
    for (size_t i = 1; i < vec->len; ++i) {
        int prev = ((int *)vec->base)[i - 1];
        int curr = ((int *)vec->base)[i];
        if (prev > curr) return 0;
    }
    return 1;
}

static void dump_tree_inorder(RedBlackTree *tree, const char *label) {
    Vector *v = initialize_vector("int", sizeof(int));
    inorder_collect(tree, v);
    printf("%s (size=%zu, vec.len=%zu):", label, tree->size, v->len);
    for (size_t i = 0; i < v->len; ++i) {
        int x = ((int *)v->base)[i];
        printf(" %d", x);
    }
    printf("\n");
    free_vector(v);
}

// Full RB-tree check after a series of operations
static void check_rbt(RedBlackTree *tree, const char *phase) {
    printf("Checking invariants: %s\n", phase);

    if (tree->root != NULL) {
        assert(tree->root->color == RBT_COLOR_BLACK &&
               "Root is not black");
    }

    check_no_red_red(tree->root);

    int bh = check_black_height(tree->root);
    assert(bh >= 0 && "Black height mismatch on some path");

    Vector *v = initialize_vector("int", sizeof(int));
    inorder_collect(tree, v);

    assert(v->len == tree->size &&
           "Inorder traversal size != tree->size (check your vector API/type)");

    assert(vector_is_sorted_int(v) && "Inorder traversal is not sorted");

    free_vector(v);
}

// ---------------------------------------------------------
// Deterministic test scenarios (existing)
// ---------------------------------------------------------

static void test_insert_simple_patterns() {
    printf("=== test_insert_simple_patterns ===\n");

    RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
    assert(t);

    int seq1[] = {30, 20, 10};
    for (size_t i = 0; i < 3; ++i) {
        push_to_redblack_tree(t, &seq1[i]);
    }
    check_rbt(t, "after LL insert 30,20,10");
    dump_tree_inorder(t, "inorder LL");

    int seq2[] = {40, 50, 25, 5};
    for (size_t i = 0; i < 4; ++i) {
        push_to_redblack_tree(t, &seq2[i]);
    }
    check_rbt(t, "after more inserts (LL side-cases)");
    dump_tree_inorder(t, "inorder LL extended");

    free_redblack_tree(t);
}

static void test_insert_LR_RL_RR_patterns() {
    printf("=== test_insert_LR_RL_RR_patterns ===\n");

    // LR pattern: 30, 10, 20
    {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        int seq[] = {30, 10, 20};
        for (size_t i = 0; i < 3; ++i) push_to_redblack_tree(t, &seq[i]);
        check_rbt(t, "LR insert 30,10,20");
        dump_tree_inorder(t, "inorder LR");
        free_redblack_tree(t);
    }

    // RL pattern: 10, 30, 20
    {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        int seq[] = {10, 30, 20};
        for (size_t i = 0; i < 3; ++i) push_to_redblack_tree(t, &seq[i]);
        check_rbt(t, "RL insert 10,30,20");
        dump_tree_inorder(t, "inorder RL");
        free_redblack_tree(t);
    }

    // RR pattern and recolors: ascending
    {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        int seq[] = {10, 20, 30, 40, 50, 60, 70};
        for (size_t i = 0; i < sizeof(seq)/sizeof(seq[0]); ++i) {
            push_to_redblack_tree(t, &seq[i]);
        }
        check_rbt(t, "ascending inserts (RR cases)");
        dump_tree_inorder(t, "inorder ascending");
        free_redblack_tree(t);
    }
}

static void test_delete_leaf_cases() {
    printf("=== test_delete_leaf_cases ===\n");
    RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
    int seq[] = {20, 10, 30, 5, 15, 25, 35};
    for (size_t i = 0; i < 7; ++i) push_to_redblack_tree(t, &seq[i]);
    check_rbt(t, "before leaf deletions");
    dump_tree_inorder(t, "inorder before leaf delete");

    int deletes[] = {5, 15, 25, 35};
    for (size_t i = 0; i < 4; ++i) {
        int key = deletes[i];
        void *removed = remove_by_value_redblack_tree(t, &key);
        assert(removed != NULL);
        free(removed);
        check_rbt(t, "after leaf deletion");
        dump_tree_inorder(t, "inorder after leaf delete step");
    }

    free_redblack_tree(t);
}

static void test_delete_one_child_cases() {
    printf("=== test_delete_one_child_cases ===\n");

    RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
    int seq[] = {20, 10, 30, 40};
    for (size_t i = 0; i < 4; ++i) push_to_redblack_tree(t, &seq[i]);
    check_rbt(t, "before one-child deletion");
    dump_tree_inorder(t, "inorder before one-child delete");

    {
        int key = 40;
        void *removed = remove_by_value_redblack_tree(t, &key);
        assert(removed != NULL);
        free(removed);
        check_rbt(t, "after deleting leaf 40");
        dump_tree_inorder(t, "inorder after delete 40");
    }

    {
        int v = 40;
        push_to_redblack_tree(t, &v);
        check_rbt(t, "after reinserting 40");
        dump_tree_inorder(t, "inorder after reinsert 40");
    }

    {
        int key = 30;
        void *removed = remove_by_value_redblack_tree(t, &key);
        assert(removed != NULL);
        free(removed);
        check_rbt(t, "after deleting 30 (one-child / internal)");
        dump_tree_inorder(t, "inorder after delete 30");
    }

    free_redblack_tree(t);
}

static void test_delete_two_children_case() {
    printf("=== test_delete_two_children_case ===\n");

    RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
    int seq[] = {20, 10, 30, 5, 15, 25, 35};
    for (size_t i = 0; i < 7; ++i) push_to_redblack_tree(t, &seq[i]);
    check_rbt(t, "before two-children deletion");
    dump_tree_inorder(t, "inorder before delete 20 (two children)");

    int key = 20;
    void *removed = remove_by_value_redblack_tree(t, &key);
    assert(removed != NULL);
    free(removed);
    check_rbt(t, "after deleting 20 (two children)");
    dump_tree_inorder(t, "inorder after delete 20");

    free_redblack_tree(t);
}

static void test_delete_root_cases() {
    printf("=== test_delete_root_cases ===\n");

    // root only
    {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        int x = 10;
        push_to_redblack_tree(t, &x);
        check_rbt(t, "single-node tree before root delete");

        int key = 10;
        void *removed = remove_by_value_redblack_tree(t, &key);
        assert(removed != NULL);
        free(removed);
        assert(t->root == NULL);
        assert(t->size == 0);
        printf("root-only deletion OK\n");

        free_redblack_tree(t);
    }

    // root with one child
    {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        int a = 10, b = 5;
        push_to_redblack_tree(t, &a);
        push_to_redblack_tree(t, &b);
        check_rbt(t, "root+one-child before root delete");

        int key = 10;
        void *removed = remove_by_value_redblack_tree(t, &key);
        assert(removed != NULL);
        free(removed);
        check_rbt(t, "after deleting old root 10");
        dump_tree_inorder(t, "inorder after delete root 10");

        free_redblack_tree(t);
    }

    // root with two children
    {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        int a = 10, b = 5, c = 15;
        push_to_redblack_tree(t, &a);
        push_to_redblack_tree(t, &b);
        push_to_redblack_tree(t, &c);
        check_rbt(t, "root+two-children before root delete");

        int key = 10;
        void *removed = remove_by_value_redblack_tree(t, &key);
        assert(removed != NULL);
        free(removed);
        check_rbt(t, "after deleting root 10 (two children)");
        dump_tree_inorder(t, "inorder after delete root 10");

        free_redblack_tree(t);
    }
}

// ---------------------------------------------------------
// EXTRA deterministic tests you asked for
// ---------------------------------------------------------

// Generate next lexicographic permutation of array a[n]; returns 0 when done
static int next_permutation(int *a, int n) {
    int i = n - 2;
    while (i >= 0 && a[i] > a[i + 1]) --i;
    if (i < 0) return 0;
    int j = n - 1;
    while (a[j] < a[i]) --j;
    int tmp = a[i]; a[i] = a[j]; a[j] = tmp;
    int l = i + 1, r = n - 1;
    while (l < r) {
        tmp = a[l]; a[l] = a[r]; a[r] = tmp;
        ++l; --r;
    }
    return 1;
}

// Test all permutations of small arrays (N up to 7 -> 5040 permutations)
static void test_all_permutations_small(int N) {
    printf("=== test_all_permutations_small N=%d ===\n", N);
    int *base = malloc(sizeof(int) * N);
    for (int i = 0; i < N; ++i) base[i] = i;

    // Copy for permutation
    int *perm = malloc(sizeof(int) * N);
    memcpy(perm, base, sizeof(int) * N);

    int count = 0;
    do {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        assert(t);

        // Insert in this permutation order
        for (int i = 0; i < N; ++i) {
            push_to_redblack_tree(t, &perm[i]);
        }
        check_rbt(t, "after permutation insert");

        // Delete them all in the same order
        for (int i = 0; i < N; ++i) {
            int key = perm[i];
            void *removed = remove_by_value_redblack_tree(t, &key);
            assert(removed != NULL);
            free(removed);
            check_rbt(t, "during permutation delete");
        }

        assert(t->size == 0);
        free_redblack_tree(t);

        ++count;
        // Uncomment if you want to see progress:
        // if (count % 1000 == 0) printf("  permutations tested: %d\n", count);
    } while (next_permutation(perm, N));

    printf("Permutations tested: %d\n", count);
    free(base);
    free(perm);
}

// Insert random, then delete in reverse-insertion order
static void test_random_then_reverse_delete(int n, int iters) {
    printf("=== test_random_then_reverse_delete (n=%d, iters=%d) ===\n", n, iters);
    srand((unsigned)time(NULL) ^ 0x12345678);

    for (int it = 0; it < iters; ++it) {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        assert(t);

        int *vals = malloc(sizeof(int) * n);
        for (int i = 0; i < n; ++i) {
            vals[i] = rand() % (n * 10);
            push_to_redblack_tree(t, &vals[i]);
            check_rbt(t, "after each random insert (reverse test)");
        }

        // delete in reverse insertion order
        for (int i = n - 1; i >= 0; --i) {
            int key = vals[i];
            void *removed = remove_by_value_redblack_tree(t, &key);
            if (removed) free(removed);
            check_rbt(t, "during reverse-order deletes");
        }

        assert(t->size == 0);
        free_redblack_tree(t);
        free(vals);
    }
}

// Reference model using sorted vector to cross-check contents
static void ref_insert(Vector *ref, int x) {
    // keep sorted unique multiset-like; here we allow duplicates
    int *arr = (int *)ref->base;
    size_t n = ref->len;
    size_t pos = 0;
    while (pos < n && arr[pos] <= x) pos++;

    // grow by one element
    int tmp = x;
    // push dummy at end
    vec_push_back(ref, &tmp);
    arr = (int *)ref->base;
    // shift elements from pos..n-1 to pos+1..n
    for (size_t i = n; i > pos; --i) {
        arr[i] = arr[i - 1];
    }
    arr[pos] = x;
}

static void ref_delete(Vector *ref, int x) {
    int *arr = (int *)ref->base;
    size_t n = ref->len;
    size_t pos = 0;
    while (pos < n && arr[pos] != x) pos++;
    if (pos == n) return; // not found

    for (size_t i = pos + 1; i < n; ++i) {
        arr[i - 1] = arr[i];
    }
    ref->len--;
}

static void ref_check_against_tree(RedBlackTree *t, Vector *ref, const char *phase) {
    Vector *v = initialize_vector("int", sizeof(int));
    inorder_collect(t, v);

    assert(v->len == ref->len && "ref vs tree size mismatch");

    int *arr_tree = (int *)v->base;
    int *arr_ref  = (int *)ref->base;
    for (size_t i = 0; i < v->len; ++i) {
        if (arr_tree[i] != arr_ref[i]) {
            printf("REF MISMATCH at %s index %zu: tree=%d ref=%d\n",
                   phase, i, arr_tree[i], arr_ref[i]);
            assert(0 && "reference vs tree content mismatch");
        }
    }

    free_vector(v);
}

static void test_mixed_operations_with_reference(int n_ops, int key_range) {
    printf("=== test_mixed_operations_with_reference (n_ops=%d, key_range=%d) ===\n",
           n_ops, key_range);

    srand((unsigned)time(NULL) ^ 0xCAFEBABE);

    RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
    assert(t);
    Vector *ref = initialize_vector("int", sizeof(int));

    for (int i = 0; i < n_ops; ++i) {
        int op = rand() % 3; // 0=insert,1=delete,2=search
        int key = rand() % key_range;

        if (op == 0) {
            // insert
            push_to_redblack_tree(t, &key);
            ref_insert(ref, key);
        } else if (op == 1) {
            // delete
            void *removed = remove_by_value_redblack_tree(t, &key);
            if (removed) free(removed);
            ref_delete(ref, key);
        } else {
            // search
            RedBlackTreeNode *node = search_redblack_tree(t, t->root, &key);
            // we won't assert on presence here, because we allow duplicates etc.
            (void)node;
        }

        check_rbt(t, "during mixed ops");
        ref_check_against_tree(t, ref, "mixed ops");
    }

    free_redblack_tree(t);
    free_vector(ref);
}

// Heavy random: insert n, then random delete order (already have something similar)
static void test_random_stress(int n, int iters) {
    printf("=== test_random_stress (n=%d, iters=%d) ===\n", n, iters);

    srand((unsigned)time(NULL));

    for (int it = 0; it < iters; ++it) {
        RedBlackTree *t = initialize_redblack_tree("int", sizeof(int), int_compare);
        assert(t);
        int *vals = malloc(sizeof(int) * n);
        assert(vals);

        for (int i = 0; i < n; ++i) {
            vals[i] = rand() % (n * 10);
            push_to_redblack_tree(t, &vals[i]);
        }

        check_rbt(t, "after random inserts");

        // random deletion order
        for (int i = 0; i < n; ++i) {
            int idx = rand() % n;
            int key = vals[idx];
            void *removed = remove_by_value_redblack_tree(t, &key);
            if (removed) free(removed);
            check_rbt(t, "during random deletions");
        }

        free_redblack_tree(t);
        free(vals);
    }
}

// ---------------------------------------------------------
// main
// ---------------------------------------------------------

int main(void) {
    printf("RED-BLACK TREE FULL TEST START\n");

    // Original structured tests
    test_insert_simple_patterns();
    test_insert_LR_RL_RR_patterns(); // typo: keep as test_insert_LR_RL_RR_patterns if using original name
    test_delete_leaf_cases();
    test_delete_one_child_cases();
    test_delete_two_children_case();
    test_delete_root_cases();

    // Extra deterministic: all permutations for small N
    // N=6 => 720 permutations, N=7 => 5040 permutations; adjust for time.
    test_all_permutations_small(6);

    // Random, then delete in reverse insertion order
    test_random_then_reverse_delete(100, 5);

    // Mixed insert/delete/search with reference model
    test_mixed_operations_with_reference(2000, 200);

    // Big random stress
    test_random_stress(200, 20);

    printf("ALL TESTS PASSED\n");
    return 0;
}
