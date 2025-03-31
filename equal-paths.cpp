#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkDepths(Node* root, int right, int left, int &leaf);

bool equalPaths(Node * root)
{
    // Add your code below
    int leaf = -1;
    return checkDepths(root, 0, 0, leaf);
}

bool checkDepths(Node* root, int left, int right, int &leaf) {
    if (root == nullptr) {
        return true;
    }
    if (root->left != nullptr) left++;
    if (root->right != nullptr) right++;
    if (root->left == nullptr && root->right == nullptr) {
        int depth;
        if (left > right) {
            depth = left;
        }
        else {
            depth = right;
        }
        if (leaf == -1) {
            leaf = depth;
        }
        return leaf == depth;
    }
    return (checkDepths(root->left, left, right, leaf) && checkDepths(root->right, left, right, leaf));
} 


