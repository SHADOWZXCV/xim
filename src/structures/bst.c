#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "structures/bst.h"

BinaryTreeNodeDirection find_node_for_insertion(BinaryTree *tree, BinaryTreeNode *currentNode, void *value);
void free_node(BinaryTreeNode *node);

BinaryTree *initialize_binary_tree(char *type, size_t type_size, short(*compare)(void *a, void *b)) {
    if (compare == NULL) {
        return NULL; // failed
    }

    BinaryTree *tree = calloc(1, sizeof(*tree));

    if (tree == NULL) {
        return NULL;
    }

    if (!strcmp(type, "char")) {
        tree->type = TYPE_CHAR;
    } else if (!strcmp(type, "int")) {
        tree->type = TYPE_INT;
    } else {
        assert(0 && "THIS TYPE IS NOT SUPPORTED FOR BINARY TREES!");
    }

    tree->compare = compare;
    tree->type_size = type_size;

    return tree;
}

BinaryTreeNode *push_to_tree(BinaryTree *tree, void *value) {
    if (tree == NULL) {
        return NULL;
    }

    BinaryTreeNode *node = calloc(1, sizeof(*node));

    if (node == NULL) {
        return NULL;
    }

    tree->size++;
    node->value = malloc(tree->type_size);

    memcpy(node->value, value, tree->type_size);

    if (tree->root == NULL) {
        tree->root = node;

        return node;
    }

    BinaryTreeNodeDirection parent = find_node_for_insertion(tree, tree->root, value);

    if (parent.node == NULL) {
        return NULL; // Bad initial root
    }

    if (parent.direction == BINARY_TREE_NODE_LEFT) {
        parent.node->left = node;
        parent.node->direction_from_parent = BINARY_TREE_NODE_LEFT;
    } else {
        parent.node->right = node;
        parent.node->direction_from_parent = BINARY_TREE_NODE_RIGHT;
    }

    node->parent = parent.node;

    return node;
}

// in-order traversal: left - node - right
//! TODO: check if the types are the same between the vector and the tree
BinaryTreeNode *iterate_binary_tree(BinaryTree *tree, BinaryTreeNode *node, Vector *container) {   
    if (tree == NULL || node == NULL) {
        return NULL;
    }

    iterate_binary_tree(tree, node->left, container);
    vec_push_back(container, node->value);
    iterate_binary_tree(tree, node->right, container);

    return node;
}

BinaryTreeNode *search_binary_tree(BinaryTree *tree, BinaryTreeNode *node, void *value) {
    if (tree == NULL || node == NULL) {
        return NULL;
    }

    short result = tree->compare(value, node->value);

    if (result < 0) {
        return search_binary_tree(tree, node->left, value);
    } else if (result > 0) {
        return search_binary_tree(tree, node->right, value);
    }

    return node;
}

void *remove_by_value_binary_tree(BinaryTree *tree, void *value) {
    if (tree == NULL) {
        return NULL;
    }

    BinaryTreeNode *node = search_binary_tree(tree, tree->root, value);

    if (node == NULL) {
        return NULL;
    }

    tree->size--;

    // case-1: It is a leaf node
    if (node->left == NULL && node->right == NULL) {
        if (node->parent != NULL) { // not a tree with only a root node
            if (node->parent->right == node) {
                node->parent->right = NULL;
            } else {
                node->parent->left = NULL;
            }
        } else {
            tree->root = NULL;
        }

        void *temp = node->value;

        free(node);

        return temp;
    }
    // case-3: it is a node with 2 children
    else if (node->left != NULL && node->right != NULL) {
        // find the in-order successor ( smallest value of the right subtree of the node )
        // get the leftmost child node of the current node
        BinaryTreeNode *min_node = node->right;

        while (min_node->left != NULL) {
            min_node = min_node->left;
        }

        void *temp = node->value;

        node->value = min_node->value;

        BinaryTreeNode *right_child = min_node->right;

        if (min_node->parent->right == min_node) {
            min_node->parent->right = right_child;
        } else if (min_node->parent->left == min_node) {
            min_node->parent->left = right_child;
        }

        if (right_child != NULL) {
            right_child->parent = min_node->parent;
        }        

        free(min_node);

        return temp;
    }  // case-2: It is a node with 1 child
    else {
        BinaryTreeNode *child_node = node->left != NULL ? node->left : node->right;
        if (node->parent == NULL) { // a root node
            tree->root = child_node;
            if (child_node != NULL) {
                child_node->parent = NULL;
            }
        } else {
            if (node->parent->right == node) { // node is right from the parent
                node->parent->right = child_node;
            } else { // node is left from the parent
                node->parent->left = child_node;
            }
            
            if (child_node != NULL)
                child_node->parent = node->parent;
        }


        void *temp = node->value;

        free(node);

        return temp;
    }
}

void right_rotate_subtree_tree(BinaryTree *tree, BinaryTreeNode *x) {
    if (x == NULL || tree == NULL) {
        return;
    }

    if (x->left == NULL) {
        return;
    }

    BinaryTreeNode *swapped_child = x->left->right; // could be null
    BinaryTreeNode *grand_parent = x->parent;
    BinaryTreeNode *left = x->left;

    x->left->parent = x->parent;
    x->left->right = x;
    x->parent = x->left;
    x->left = swapped_child;

    if (swapped_child != NULL) {
        swapped_child->parent = x;
    }

    // after rotation
    if (grand_parent == NULL) {
        tree->root = x->parent;
        return;
    }

    if (grand_parent->left == x) {
        grand_parent->left = left;
    } else if (grand_parent->right == x) {
        grand_parent->right = left;
    }
}

void left_rotate_subtree_tree(BinaryTree *tree, BinaryTreeNode *x) {
    if (x == NULL || tree == NULL) {
        return;
    }

    if (x->right == NULL) {
        return;
    }

    BinaryTreeNode *swapped_child = x->right->left; // could be null
    BinaryTreeNode *grand_parent = x->parent;
    BinaryTreeNode *right = x->right;

    x->right->parent = x->parent;
    x->right->left = x;
    x->parent = x->right;
    x->right = swapped_child;

    if (swapped_child != NULL) {
        swapped_child->parent = x;
    }

    // after rotation
    if (grand_parent == NULL) {
        tree->root = x->parent;
        return;
    }

    if (grand_parent->left == x) {
        grand_parent->left = right;
    } else if (grand_parent->right == x) {
        grand_parent->right = right;
    }
}

BinaryTreeNodeDirection find_node_for_insertion(BinaryTree *tree, BinaryTreeNode *currentNode, void *value) {
    if (tree == NULL || currentNode == NULL) {
        return (BinaryTreeNodeDirection) {.node = NULL, .direction = BINARY_TREE_NODE_NONE }; // Bad current node
    }

    // inserted value > node value or equal
    if (tree->compare(value, currentNode->value) > -1) {
        if (currentNode->right == NULL) {
            return (BinaryTreeNodeDirection) {.node = currentNode, .direction = BINARY_TREE_NODE_RIGHT };
        }

        return find_node_for_insertion(tree, currentNode->right, value);
    } else {
        if (currentNode->left == NULL) {
            return (BinaryTreeNodeDirection) {.node = currentNode, .direction = BINARY_TREE_NODE_LEFT };
        }

        return find_node_for_insertion(tree, currentNode->left, value);
    }
}

void free_tree(BinaryTree *tree) {
    if (tree == NULL) {
        return;
    }

    free_node(tree->root);
    free(tree);
}

void free_node(BinaryTreeNode *node) {
    if (node == NULL) {
        return;
    }

    free_node(node->left);
    free_node(node->right);
    free(node->value);
    free(node);
}
