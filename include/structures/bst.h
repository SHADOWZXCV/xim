#ifndef BST_H_
#define BST_H_
#include <memory.h>
#include <assert.h>
#include <string.h>
#include "types.h"
#include "structures/vector.h"

typedef struct BinaryTreeNode BinaryTreeNode;
struct BinaryTreeNode {
    void *value;
    BinaryTreeNode *left;
    BinaryTreeNode *right;
    BinaryTreeNode *parent;
    enum BINARY_TREE_NODE_DIRECTION direction_from_parent;
};

enum BINARY_TREE_NODE_DIRECTION {
    BINARY_TREE_NODE_LEFT = 0,
    BINARY_TREE_NODE_RIGHT,
    BINARY_TREE_NODE_NONE
};

typedef struct {
    BinaryTreeNode *node;
    enum BINARY_TREE_NODE_DIRECTION direction;
} BinaryTreeNodeDirection;

typedef struct {
    enum DATA_TYPES type;
    size_t type_size;
    size_t size;
    short(*compare)(void *a, void *b);
    BinaryTreeNode *root;
} BinaryTree;

BinaryTree *initialize_binary_tree(char *type, size_t type_size, short(*compare)(void *a, void *b));
BinaryTreeNode *push_to_tree(BinaryTree *tree, void *value);
BinaryTreeNode *iterate_binary_tree(BinaryTree *tree, BinaryTreeNode *node, Vector *container);
BinaryTreeNode *search_binary_tree(BinaryTree *tree, BinaryTreeNode *node, void *value);
void *remove_by_value_binary_tree(BinaryTree *tree, void *value);
void free_tree(BinaryTree *tree);

#endif
