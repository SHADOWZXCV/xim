#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "structures/rbt.h"

RedBlackTreeNodeDirection find_redblack_node_for_insertion(RedBlackTree *tree, RedBlackTreeNode *currentNode, void *value);
RedBlackTreeNode *cut_node_from_tree_by_value(RedBlackTree *tree, void *value);
void fix_red_violations(RedBlackTree *tree, RedBlackTreeNode *node);
void fix_black_violations(RedBlackTree *tree, RedBlackTreeNode *x, RedBlackTreeNode *x_parent);
void free_redblack_node(RedBlackTreeNode *node);
void right_rotate_redblack_subtree(RedBlackTree *tree, RedBlackTreeNode *x);
void left_rotate_redblack_subtree(RedBlackTree *tree, RedBlackTreeNode *x);

RedBlackTree *initialize_redblack_tree(char *type, size_t type_size, short(*compare)(void *a, void *b)) {
    if (compare == NULL) {
        return NULL;
    }

    RedBlackTree *tree = calloc(1, sizeof(*tree));

    if (tree == NULL) {
        return NULL;
    }

    if (! strcmp(type, "char")) {
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

RedBlackTreeNode *push_to_redblack_tree(RedBlackTree *tree, void *value) {
    if (tree == NULL) {
        return NULL;
    }

    RedBlackTreeNode *node = calloc(1, sizeof(*node));

    if (node == NULL) {
        return NULL;
    }

    tree->size++;
    node->color = RBT_COLOR_RED;
    node->value = malloc(tree->type_size);

    memcpy(node->value, value, tree->type_size);

    if (tree->root == NULL) {
        tree->root = node;
        node->color = RBT_COLOR_BLACK;
        return node;
    }

    RedBlackTreeNodeDirection nodeObj = find_redblack_node_for_insertion(tree, tree->root, value);

    if (nodeObj. node == NULL) {
        return NULL;
    }

    if (nodeObj.direction == BINARY_TREE_NODE_LEFT) {
        nodeObj.node->left = node;
        node->direction_from_parent = BINARY_TREE_NODE_LEFT;
    } else {
        nodeObj.node->right = node;
        node->direction_from_parent = BINARY_TREE_NODE_RIGHT;
    }

    node->parent = nodeObj.node;

    fix_red_violations(tree, node);

    return node;
}

void fix_red_violations(RedBlackTree *tree, RedBlackTreeNode *node) {
    while (node != NULL && node->parent != NULL && node->parent->color == RBT_COLOR_RED) {
        RedBlackTreeNode *parent = node->parent;
        RedBlackTreeNode *grandparent = parent->parent;

        // parent is red and has a parent -> grandparent must exist in a valid RB tree
        if (grandparent == NULL) {
            // Safety: color parent black and stop
            parent->color = RBT_COLOR_BLACK;
            break;
        }

        int parent_is_left = (grandparent->left == parent);
        RedBlackTreeNode *uncle = parent_is_left ? grandparent->right : grandparent->left;

        // Case 1: uncle is red -> recolor and move up
        if (uncle != NULL && uncle->color == RBT_COLOR_RED) {
            parent->color = RBT_COLOR_BLACK;
            uncle->color = RBT_COLOR_BLACK;
            grandparent->color = RBT_COLOR_RED;
            node = grandparent;
            continue;
        }

        // Case 2–4: uncle is black (or NULL)
        if (parent_is_left) {
            // parent is left child
            if (node == parent->right) {
                // LR: rotate parent left to get LL
                left_rotate_redblack_subtree(tree, parent);
                node = parent;          // after rotation, old parent moved down
                parent = node->parent;  // update parent
            }
            // Now LL: rotate grandparent right
            right_rotate_redblack_subtree(tree, grandparent);

            // Recolor: new parent (former parent) becomes black, grandparent red
            parent->color = RBT_COLOR_BLACK;
            grandparent->color = RBT_COLOR_RED;
        } else {
            // parent is right child
            if (node == parent->left) {
                // RL: rotate parent right to get RR
                right_rotate_redblack_subtree(tree, parent);
                node = parent;
                parent = node->parent;
            }
            // Now RR: rotate grandparent left
            left_rotate_redblack_subtree(tree, grandparent);

            parent->color = RBT_COLOR_BLACK;
            grandparent->color = RBT_COLOR_RED;
        }

        // Once we’ve done the rotations & recolor, we’re done for this insertion
        break;
    }

    if (tree->root != NULL) {
        tree->root->color = RBT_COLOR_BLACK;
    }
}

void fix_black_violations(RedBlackTree *tree, RedBlackTreeNode *x, RedBlackTreeNode *x_parent) {
    while ((x != tree->root) &&
           (x == NULL || x->color == RBT_COLOR_BLACK)) {

        if (x == x_parent->left) {
            RedBlackTreeNode *w = x_parent->right;

            // Case 1: w is red
            if (w != NULL && w->color == RBT_COLOR_RED) {
                w->color = RBT_COLOR_BLACK;
                x_parent->color = RBT_COLOR_RED;
                left_rotate_redblack_subtree(tree, x_parent);
                w = x_parent->right;
            }

            // Now w is black
            int w_left_black  = (w == NULL || w->left  == NULL || w->left->color  == RBT_COLOR_BLACK);
            int w_right_black = (w == NULL || w->right == NULL || w->right->color == RBT_COLOR_BLACK);

            // Case 2: w black and both children black
            if (w_left_black && w_right_black) {
                if (w != NULL)
                    w->color = RBT_COLOR_RED;
                x = x_parent;
                x_parent = x_parent->parent;
            } else {
                // Case 3: w black, w->right black (so w->left is red)
                if (w != NULL &&
                    (w->right == NULL || w->right->color == RBT_COLOR_BLACK)) {
                    if (w->left != NULL)
                        w->left->color = RBT_COLOR_BLACK;
                    w->color = RBT_COLOR_RED;
                    right_rotate_redblack_subtree(tree, w);
                    w = x_parent->right;
                }

                // Case 4: w black, w->right red
                if (w != NULL) {
                    w->color = x_parent->color;
                    if (w->right != NULL)
                        w->right->color = RBT_COLOR_BLACK;
                }
                x_parent->color = RBT_COLOR_BLACK;
                left_rotate_redblack_subtree(tree, x_parent);
                x = tree->root;
                break;
            }
        } else {
            // Mirror image: x is right child
            RedBlackTreeNode *w = x_parent->left;

            // Case 1: w is red
            if (w != NULL && w->color == RBT_COLOR_RED) {
                w->color = RBT_COLOR_BLACK;
                x_parent->color = RBT_COLOR_RED;
                right_rotate_redblack_subtree(tree, x_parent);
                w = x_parent->left;
            }

            int w_left_black  = (w == NULL || w->left  == NULL || w->left->color  == RBT_COLOR_BLACK);
            int w_right_black = (w == NULL || w->right == NULL || w->right->color == RBT_COLOR_BLACK);

            // Case 2: w black and both children black
            if (w_left_black && w_right_black) {
                if (w != NULL)
                    w->color = RBT_COLOR_RED;
                x = x_parent;
                x_parent = x_parent->parent;
            } else {
                // Case 3: w black, w->left black (so w->right is red)
                if (w != NULL &&
                    (w->left == NULL || w->left->color == RBT_COLOR_BLACK)) {
                    if (w->right != NULL)
                        w->right->color = RBT_COLOR_BLACK;
                    w->color = RBT_COLOR_RED;
                    left_rotate_redblack_subtree(tree, w);
                    w = x_parent->left;
                }

                // Case 4: w black, w->left red
                if (w != NULL) {
                    w->color = x_parent->color;
                    if (w->left != NULL)
                        w->left->color = RBT_COLOR_BLACK;
                }
                x_parent->color = RBT_COLOR_BLACK;
                right_rotate_redblack_subtree(tree, x_parent);
                x = tree->root;
                break;
            }
        }

        if (x == NULL)
            break;
    }

    if (x != NULL)
        x->color = RBT_COLOR_BLACK;

    if (tree->root != NULL)
        tree->root->color = RBT_COLOR_BLACK;
}

RedBlackTreeNode *iterate_redblack_tree(RedBlackTree *tree, RedBlackTreeNode *node, Vector *container) {
    if (tree == NULL || node == NULL) {
        return NULL;
    }

    iterate_redblack_tree(tree, node->left, container);
    vec_push_back(container, node->value);
    iterate_redblack_tree(tree, node->right, container);

    return node;
}

RedBlackTreeNode *search_redblack_tree(RedBlackTree *tree, RedBlackTreeNode *node, void *value) {
    if (tree == NULL || node == NULL) {
        return NULL;
    }

    short result = tree->compare(value, node->value);

    if (result < 0) {
        return search_redblack_tree(tree, node->left, value);
    } else if (result > 0) {
        return search_redblack_tree(tree, node->right, value);
    }

    return node;
}

void *remove_by_value_redblack_tree(RedBlackTree *tree, void *value) {
    RedBlackTreeNode *removed_node = cut_node_from_tree_by_value(tree, value);

    if (! removed_node) return NULL;

    void *removed_value = removed_node->value;

    tree->size--;
    free(removed_node);

    return removed_value;
}

RedBlackTreeNode *cut_node_from_tree_by_value(RedBlackTree *tree, void *value) {
    if (tree == NULL) {
        return NULL;
    }

    // target = node whose value matches the key to remove
    RedBlackTreeNode *target = search_redblack_tree(tree, tree->root, value);
    if (target == NULL) {
        return NULL;
    }

    // replacement = node that will physically be removed from the tree
    // (may be target itself, or its in-order successor)
    RedBlackTreeNode *replacement = target;
    RedBlackTreeNode *fix_node = NULL;     // node where "extra black" might end up
    RedBlackTreeNode *fix_parent = NULL;   // parent used by fix_black_violations
    enum RBT_COLOR replacement_original_color = replacement->color;

    // Case 1: target has no left child (zero or one child on the right)
    if (target->left == NULL) {
        fix_node = target->right;
        fix_parent = target->parent;

        // Replace target with its right child
        if (target->parent == NULL) {
            tree->root = target->right;
        } else if (target == target->parent->left) {
            target->parent->left = target->right;
        } else {
            target->parent->right = target->right;
        }

        if (target->right != NULL) {
            target->right->parent = target->parent;
        }
    }
    // Case 2: target has no right child (zero or one child on the left)
    else if (target->right == NULL) {
        fix_node = target->left;
        fix_parent = target->parent;

        // Replace target with its left child
        if (target->parent == NULL) {
            tree->root = target->left;
        } else if (target == target->parent->left) {
            target->parent->left = target->left;
        } else {
            target->parent->right = target->left;
        }

        if (target->left != NULL) {
            target->left->parent = target->parent;
        }
    }
    // Case 3: target has two children
    else {
        // Find in-order successor: smallest node in target->right subtree
        replacement = target->right;
        while (replacement->left != NULL) {
            replacement = replacement->left;
        }

        replacement_original_color = replacement->color;
        fix_node = replacement->right;

        if (replacement->parent == target) {
            // Successor is direct right child of target
            fix_parent = replacement;
            if (fix_node != NULL) {
                fix_node->parent = replacement;
            }
        } else {
            // Detach replacement from its current location:
            // replace it with its right child
            if (replacement->parent->left == replacement) {
                replacement->parent->left = replacement->right;
            } else {
                replacement->parent->right = replacement->right;
            }
            if (replacement->right != NULL) {
                replacement->right->parent = replacement->parent;
            }

            fix_parent = replacement->parent;

            // Move target->right subtree under replacement
            replacement->right = target->right;
            if (replacement->right != NULL) {
                replacement->right->parent = replacement;
            }
        }

        // Move replacement into target's position in the tree
        if (target->parent == NULL) {
            tree->root = replacement;
        } else if (target == target->parent->left) {
            target->parent->left = replacement;
        } else {
            target->parent->right = replacement;
        }
        replacement->parent = target->parent;

        // Move target->left subtree under replacement
        replacement->left = target->left;
        if (replacement->left != NULL) {
            replacement->left->parent = replacement;
        }

        // replacement inherits target's color
        replacement->color = target->color;
    }

    // If the physically removed node was black, we may have to fix black-height
    if (replacement_original_color == RBT_COLOR_BLACK) {
        if (fix_node != NULL || fix_parent != NULL) {
            fix_black_violations(tree, fix_node, fix_parent);
        }
    }

    // Disconnect target from the tree; its value is returned to caller
    target->parent = target->left = target->right = NULL;
    return target;
}

void right_rotate_redblack_subtree(RedBlackTree *tree, RedBlackTreeNode *x) {
    if (x == NULL || tree == NULL) {
        return;
    }

    if (x->left == NULL) {
        return;
    }

    RedBlackTreeNode *swapped_child = x->left->right;
    RedBlackTreeNode *grand_parent = x->parent;
    RedBlackTreeNode *left = x->left;

    x->left->parent = x->parent;
    x->left->right = x;
    x->parent = x->left;
    x->left = swapped_child;

    if (swapped_child != NULL) {
        swapped_child->parent = x;
    }

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

void left_rotate_redblack_subtree(RedBlackTree *tree, RedBlackTreeNode *x) {
    if (x == NULL || tree == NULL) {
        return;
    }

    if (x->right == NULL) {
        return;
    }

    RedBlackTreeNode *swapped_child = x->right->left;
    RedBlackTreeNode *grand_parent = x->parent;
    RedBlackTreeNode *right = x->right;

    x->right->parent = x->parent;
    x->right->left = x;
    x->parent = x->right;
    x->right = swapped_child;

    if (swapped_child != NULL) {
        swapped_child->parent = x;
    }

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

RedBlackTreeNodeDirection find_redblack_node_for_insertion(RedBlackTree *tree, RedBlackTreeNode *currentNode, void *value) {
    if (tree == NULL || currentNode == NULL) {
        return (RedBlackTreeNodeDirection) {. node = NULL, .direction = BINARY_TREE_NODE_NONE};
    }

    if (tree->compare(value, currentNode->value) > -1) {
        if (currentNode->right == NULL) {
            return (RedBlackTreeNodeDirection) {.node = currentNode, .direction = BINARY_TREE_NODE_RIGHT};
        }

        return find_redblack_node_for_insertion(tree, currentNode->right, value);
    } else {
        if (currentNode->left == NULL) {
            return (RedBlackTreeNodeDirection) {.node = currentNode, .direction = BINARY_TREE_NODE_LEFT};
        }

        return find_redblack_node_for_insertion(tree, currentNode->left, value);
    }
}

void free_redblack_tree(RedBlackTree *tree) {
    if (tree == NULL) {
        return;
    }

    free_redblack_node(tree->root);
    free(tree);
}

void free_redblack_node(RedBlackTreeNode *node) {
    if (node == NULL) {
        return;
    }

    free_redblack_node(node->left);
    free_redblack_node(node->right);
    free(node->value);
    free(node);
}
