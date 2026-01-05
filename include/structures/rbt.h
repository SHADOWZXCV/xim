#ifndef RBT_H_
#define RBT_H_
#include "types.h"
#include "structures/vector.h"
#include "structures/bst.h"

enum RBT_COLOR {
    RBT_COLOR_RED,
    RBT_COLOR_BLACK
};

typedef struct RedBlackTreeNode RedBlackTreeNode;
typedef struct {
    RedBlackTreeNode *node;
    enum BINARY_TREE_NODE_DIRECTION direction;
} RedBlackTreeNodeDirection;

typedef struct {
    enum DATA_TYPES type;
    size_t type_size;
    size_t size;
    short(*compare)(void *a, void *b);
    RedBlackTreeNode *root;
} RedBlackTree;

struct RedBlackTreeNode {
    void *value;
    enum RBT_COLOR color;
    RedBlackTreeNode *left;
    RedBlackTreeNode *right;
    RedBlackTreeNode *parent;
    enum BINARY_TREE_NODE_DIRECTION direction_from_parent;
};

RedBlackTree *initialize_redblack_tree(char *type, size_t type_size, short(*compare)(void *a, void *b));
RedBlackTreeNode *push_to_redblack_tree(RedBlackTree *tree, void *value);
RedBlackTreeNode *iterate_redblack_tree(RedBlackTree *tree, RedBlackTreeNode *node, Vector *container);
RedBlackTreeNode *search_redblack_tree(RedBlackTree *tree, RedBlackTreeNode *node, void *value);
void *remove_by_value_redblack_tree(RedBlackTree *tree, void *value);
void free_redblack_tree(RedBlackTree *tree);

#endif