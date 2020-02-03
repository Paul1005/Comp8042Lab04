#include <iostream>
#include <cstdlib>
#include <gtest/gtest.h>
#include <stack>
#include <string>
#include <algorithm>
#include <random>
#include "Node.h"

using namespace std;

// The following few lines are to override the << operator
#include "Node.h"

typedef Node<int> Tree;

Tree* make_empty(Tree* t)
{
    if (t != NULL)
    {
        make_empty(t->left);
        make_empty(t->right);
        free(t);
    }

    return NULL;
}

Tree* find_min(Tree* t)
{
    if (t == NULL)
    {
        return NULL;
    }
    else if (t->left == NULL)
    {
        return t;
    }
    else
    {
        return find_min(t->left);
    }
}

Tree* find_max(Tree* t)
{
    if (t == NULL)
    {
        return NULL;
    }
    else if (t->right == NULL)
    {
        return t;
    }
    else
    {
        return find_max(t->right);
    }
}

Tree* find(int elem, Tree* t)
{
    if (t == NULL)
    {
        return NULL;
    }

    if (elem < t->value)
    {
        return find(elem, t->left);
    }
    else if (elem > t->value)
    {
        return find(elem, t->right);
    }
    else
    {
        return t;
    }
}

//Insert i into the tree t, duplicate will be discarded
//Return a pointer to the resulting tree.                 
Tree* insert(int value, Tree* t)
{
    Tree* new_node;

    if (t == NULL)
    {
        new_node = (Tree*)malloc(sizeof(Tree));
        if (new_node == NULL)
        {
            return t;
        }

        new_node->value = value;

        new_node->left = new_node->right = NULL;
        return new_node;
    }

    if (value < t->value)
    {
        t->left = insert(value, t->left);
    }
    else if (value > t->value)
    {
        t->right = insert(value, t->right);
    }
    else
    {
        //duplicate, ignore it
        return t;
    }
    return t;
}

Tree* remove(int value, Tree* t)
{
    //Deletes node from the tree
    // Return a pointer to the resulting tree
    Tree* x;
    Tree* tmp_cell;

    if (t == NULL) return NULL;

    if (value < t->value)
    {
        t->left = remove(value, t->left);
    }
    else if (value > t->value)
    {
        t->right = remove(value, t->right);
    }
    else if (t->left && t->right)
    {
        tmp_cell = find_min(t->right);
        t->value = tmp_cell->value;
        t->right = remove(t->value, t->right);
    }
    else
    {
        tmp_cell = t;
        if (t->left == NULL)
            t = t->right;
        else if (t->right == NULL)
            t = t->left;
        free(tmp_cell);
    }

    return t;
}


//printing tree in ascii

typedef struct asciinode_struct asciinode;

struct asciinode_struct
{
    asciinode* left, * right;

    //length of the edge from this node to its children
    int edge_length;

    int height;

    int lablen;

    //-1=I am left, 0=I am root, 1=right   
    int parent_dir;

    //max supported unit32 in dec, 10 digits max
    char label[11];
};


#define MAX_HEIGHT 1000
int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];

//adjust gap between left and right nodes
int gap = 3;

//used for printing next node in the same level, 
//this is the x coordinate of the next char printed
int print_next;

int MIN(int X, int Y)
{
    return ((X) < (Y)) ? (X) : (Y);
}

int MAX(int X, int Y)
{
    return ((X) > (Y)) ? (X) : (Y);
}

asciinode* build_ascii_tree_recursive(Tree* t)
{
    asciinode* node;

    if (t == NULL) return NULL;

    node = new asciinode();
    node->left = build_ascii_tree_recursive(t->left);
    node->right = build_ascii_tree_recursive(t->right);

    if (node->left != NULL)
    {
        node->left->parent_dir = -1;
    }

    if (node->right != NULL)
    {
        node->right->parent_dir = 1;
    }

    sprintf_s(node->label, "%d", t->value);
    node->lablen = strlen(node->label);

    return node;
}


//Copy the tree into the ascii node structre
asciinode* build_ascii_tree(Tree* t)
{
    asciinode* node;
    if (t == NULL) return NULL;
    node = build_ascii_tree_recursive(t);
    node->parent_dir = 0;
    return node;
}

//Free all the nodes of the given tree
void free_ascii_tree(asciinode* node)
{
    if (node == NULL) return;
    free_ascii_tree(node->left);
    free_ascii_tree(node->right);
    free(node);
}

//The following function fills in the lprofile array for the given tree.
//It assumes that the center of the label of the root of this tree
//is located at a position (x,y).  It assumes that the edge_length
//fields have been computed for this tree.
void compute_lprofile(asciinode* node, int x, int y)
{
    int i, isleft;
    if (node == NULL) return;
    isleft = (node->parent_dir == -1);
    lprofile[y] = MIN(lprofile[y], x - ((node->lablen - isleft) / 2));
    if (node->left != NULL)
    {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++)
        {
            lprofile[y + i] = MIN(lprofile[y + i], x - i);
        }
    }
    compute_lprofile(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
    compute_lprofile(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
}

void compute_rprofile(asciinode* node, int x, int y)
{
    int i, notleft;
    if (node == NULL) return;
    notleft = (node->parent_dir != -1);
    rprofile[y] = MAX(rprofile[y], x + ((node->lablen - notleft) / 2));
    if (node->right != NULL)
    {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++)
        {
            rprofile[y + i] = MAX(rprofile[y + i], x + i);
        }
    }
    compute_rprofile(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
    compute_rprofile(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
}

//This function fills in the edge_length and 
//height fields of the specified tree
void compute_edge_lengths(asciinode* node)
{
    int h, hmin, i, delta;
    if (node == NULL) return;
    compute_edge_lengths(node->left);
    compute_edge_lengths(node->right);

    /* first fill in the edge_length of node */
    if (node->right == NULL && node->left == NULL)
    {
        node->edge_length = 0;
    }
    else
    {
        if (node->left != NULL)
        {
            for (i = 0; i < node->left->height && i < MAX_HEIGHT; i++)
            {
                rprofile[i] = -INFINITY;
            }
            compute_rprofile(node->left, 0, 0);
            hmin = node->left->height;
        }
        else
        {
            hmin = 0;
        }
        if (node->right != NULL)
        {
            for (i = 0; i < node->right->height && i < MAX_HEIGHT; i++)
            {
                lprofile[i] = INFINITY;
            }
            compute_lprofile(node->right, 0, 0);
            hmin = MIN(node->right->height, hmin);
        }
        else
        {
            hmin = 0;
        }
        delta = 4;
        for (i = 0; i < hmin; i++)
        {
            delta = MAX(delta, gap + 1 + rprofile[i] - lprofile[i]);
        }

        //If the node has two children of height 1, then we allow the
        //two leaves to be within 1, instead of 2 
        if (((node->left != NULL && node->left->height == 1) ||
            (node->right != NULL && node->right->height == 1)) && delta > 4)
        {
            delta--;
        }

        node->edge_length = ((delta + 1) / 2) - 1;
    }

    //now fill in the height of node
    h = 1;
    if (node->left != NULL)
    {
        h = MAX(node->left->height + node->edge_length + 1, h);
    }
    if (node->right != NULL)
    {
        h = MAX(node->right->height + node->edge_length + 1, h);
    }
    node->height = h;
}

//This function prints the given level of the given tree, assuming
//that the node has the given x cordinate.
void print_level(std::ostream& out, asciinode* node, int x, int level)
{
    int i, isleft;
    if (node == NULL) return;
    isleft = (node->parent_dir == -1);
    if (level == 0)
    {
        for (i = 0; i < (x - print_next - ((node->lablen - isleft) / 2)); i++)
        {
            printf(" ");
        }
        print_next += i;
        out << node->label;
        //printf("%s", node->label);
        print_next += node->lablen;
    }
    else if (node->edge_length >= level)
    {
        if (node->left != NULL)
        {
            for (i = 0; i < (x - print_next - (level)); i++)
            {
                out << " ";
                //printf(" ");
            }
            print_next += i;
            out << "/";
            //printf("/");
            print_next++;
        }
        if (node->right != NULL)
        {
            for (i = 0; i < (x - print_next + (level)); i++)
            {
                out << " ";
                //printf(" ");
            }
            print_next += i;
            out << "\\";
            //printf("\\");
            print_next++;
        }
    }
    else
    {
        print_level(out, node->left,
            x - node->edge_length - 1,
            level - node->edge_length - 1);
        print_level(out, node->right,
            x + node->edge_length + 1,
            level - node->edge_length - 1);
    }
}

//prints ascii tree for given Tree structure
void print_ascii_tree(std::ostream& out, Tree* t)
{
    asciinode* proot;
    int xmin, i;
    if (t == NULL) return;
    proot = build_ascii_tree(t);
    compute_edge_lengths(proot);
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
    {
        lprofile[i] = INFINITY;
    }
    compute_lprofile(proot, 0, 0);
    xmin = 0;
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
    {
        xmin = MIN(xmin, lprofile[i]);
    }
    for (i = 0; i < proot->height; i++)
    {
        print_next = 0;
        print_level(out, proot, -xmin, i);
        printf("\n");
    }
    if (proot->height >= MAX_HEIGHT)
    {
        printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
    }
    free_ascii_tree(proot);
}


template <typename T>
class AVL;

template <typename T>
std::ostream& operator<< (std::ostream& out, const AVL<T>& v);

template <typename T>
class AVL {
    Node<T>* root;

    vector<Node<T>*> searchNode(const T& x) {
        vector<Node<T>*> path;
        Node<T>* p = root;
        while (p != NULL) {
            path.push_back(p);
            if (p->value < x)
                p = p->right;
            else if (x < p->value)
                p = p->left;
            else
                return path;  // Element found in the tree, return the full path from the root to the node
        }
        return path; // Element not found in the tree, return the path to where the node is supposed to be
    }
public:
    AVL() : root(NULL) {}
    ~AVL() {
        // Delete all nodes in the tree
        stack<Node<T>*> toDelete;
        toDelete.push(root);
        while (!toDelete.empty()) {
            Node<T>* node = toDelete.top();
            toDelete.pop();
            if (node != NULL) {
                delete node;
                toDelete.push(node->left);
                toDelete.push(node->right);
            }
        }
    }

    bool insert(const T& x) {
        vector<Node<T>*> p = searchNode(x);
        if (p.empty()) {
            // Inserting at the root
            root = new Node<T>(x);
            // No need to check the balance after inserting one node
            return true;
        }
        else {
            // Inserting at a non-root node
            Node<T>* newNode = new Node<T>(x);
            Node<T>* parent = p.back();
            if (parent->value < x)
                parent->right = newNode;
            else
                parent->left = newNode;
            p.push_back(newNode);
            // Now we need to update the tree height and check the balance
            for (int i = p.size() - 1; i >= 0; i--) {
                p[i]->updateHeight();
                if (!p[i]->balanced()) {
                    // Balance the tree after insertion
                    // TODO Perform the correct rotation to balance p[i]
                    // TODO Do not forget to update the tree link pointing to p[i] after the rotation is done
                    // TODO Do not forget to update the height of the tree nodes after the rotation
                    //      to ensure that future insert operations will work correctly
                }
            }
            //TODO this line is just to prevent the warning:
            //      ``control may reach end of non-void function``
            return false;
        }
    }

    bool search(const T& x) {
        vector<Node<T>*> p = searchNode(x);
        return !p.empty() && p.back()->value == x;
    }

    /// Return true if the tree is empty
    bool empty() {
        return root == NULL;
    }

    /// Check if all the nodes in the tree are balanced according to the AVL balance condition
    bool balanced() {
        stack<Node<T>*> toTest;
        toTest.push(root);
        while (!toTest.empty()) {
            Node<T>* n = toTest.top();
            toTest.pop();
            if (n != NULL) {
                if (!n->balanced())
                    return false;
                toTest.push(n->left);
                toTest.push(n->right);
            }
        }
        return true;
    }

    /// Make the << function a friend function to ensure it has access to root
    friend std::ostream& operator<< <> (std::ostream& out, const AVL<T>& v);
};

template <typename T>
std::ostream& operator<< (std::ostream& out, const AVL<T>& v) {
    print_ascii_tree(out, v.root);
    return out;
}

int main() {
    // HINT: Use cout << avl to print the tree after each insertion or rotate.
    //       It might make it more convenient to debug and understand what is going on.
    // Simple test for the four cases after insertion
    AVL<int> avl;
    avl.insert(10);
    avl.insert(9);
    avl.insert(7); // A single right rotation should happen here
    EXPECT_TRUE(avl.balanced());
    avl.insert(14);
    avl.insert(18);
    EXPECT_TRUE(avl.balanced());
    avl.insert(11); // A double rotation should happen here
    EXPECT_TRUE(avl.balanced());
    avl.insert(8);
    EXPECT_TRUE(avl.balanced());

    // A good way to test an AVL tree is to create a worst-case scenario by
    // inserting items in the sorted order and make sure that the tree is
    // balanced after each insertion
    AVL<int> seq_avl;
    for (int i = 1; i <= 20; i++) {
        seq_avl.insert(i);
        EXPECT_TRUE(seq_avl.balanced());
    }

    // Print the final tree
    cout << seq_avl << endl;
    return 0;
}
