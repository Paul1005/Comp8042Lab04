#include <iostream>
#include <cstdlib>
#include <stack>
#include <string>
#include <algorithm>
#include <random>

#pragma once
/// This macro makes it more convenient to retrieve the height of an empty subtree
#define HEIGHT(n) (((n) == NULL)? -1 : ((n)->height))

template <typename T>
struct Node {
    T value;
    int height;
    Node<T>* left, * right;
    Node() : left(NULL), right(NULL), height(0) {}
    Node(const T& x) : value(x), left(NULL), right(NULL), height(0) {}

    /// Update the height of the subtree rooted at this node assuming that the
    /// two subtrees have the correct height
    void updateHeight() {
        height = std::max(HEIGHT(left), HEIGHT(right)) + 1;
    }

    /// Check if this node is balanced according to the AVL tree condition
    bool balanced() {
        return abs(HEIGHT(left) - HEIGHT(right)) <= 1;
    }

    // Perform a single rotation at the current node and return the new root after rotation
    Node* singleLeftRotation() {
        // TODO Update the links as mandated by this rotation
        // TODO Do not forget to udpate the tree height for the updated subtrees
        // TODO Make sure to return the updated root after rotation
		Node* y = right;
		Node* T2 = y->left;

		// Perform rotation  
		y->left = this;
		right = T2;

        return y;
    }

    // Perform a single rotation at the current node and return the new root after rotation
    Node* singleRightRotation() {
        // TODO Update the links as mandated by this rotation
        // TODO Do not forget to udpate the tree height for the updated subtrees
        // TODO Make sure to return the updated root after rotation
		Node* x = left;
		Node* T2 = x->right;

		// Perform rotation  
		x->right = this;
		left = T2;

        return x;
    }

    // Perform a double rotation at the current node and return the new root after rotation
    Node* doubleLeftRightRotation() {
        // TODO Update the links as mandated by this rotation
        // TODO Do not forget to udpate the tree height for the updated subtrees
        // TODO Make sure to return the updated root after rotation
		left = left->singleLeftRotation();
		Node* x = singleRightRotation();
        return x;
    }

    // Perform a double rotation at the current node and return the new root after rotation
    Node* doubleRightLeftRotation() {
        // TODO Update the links as mandated by this rotation
        // TODO Do not forget to udpate the tree height for the updated subtrees
        // TODO Make sure to return the updated root after rotation
		right = right->singleRightRotation();
		Node* y = singleLeftRotation();
        return y;
    }
};

