#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* node);
    void rotateLeft(AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* n, int diff);
    static AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* curr);
    AVLNode<Key, Value>* internalFind(const Key& k) const;
    void updateLinks(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* oldChild, AVLNode<Key, Value>* newChild);
};

template<class Key, class Value>
void AVLTree<Key, Value>::updateLinks(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* oldChild, AVLNode<Key, Value>* newChild) {
  if (parent == nullptr) {
    this->root_ = newChild;
  }
  else if (parent->getLeft() == oldChild) {
    parent->setLeft(newChild);
  }
  else if (parent->getRight() == oldChild) {
    parent->setRight(newChild);
  }
  if (newChild != nullptr) {
    newChild->setParent(parent);
  }
}
/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* toInsert = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);

    if (this->root_ == nullptr) {
      this->root_ = toInsert;
      return;
    }
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (curr != nullptr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
          curr = curr->getLeft();
        }
        else if (new_item.first > curr->getKey()) {
          curr = curr->getRight();
        }
        else {
          curr->setValue(new_item.second);
          delete toInsert;
          return;
        }
    }
    toInsert->setParent(parent);
    //now we're at a leaf node
    if (new_item.first < parent->getKey()) {
        parent->setLeft(toInsert);
        toInsert->setParent(parent);
        parent->setBalance(parent->getBalance() - 1);
    }
    else {
        parent->setRight(toInsert);
        toInsert->setParent(parent);
        parent->setBalance(parent->getBalance() + 1);
    }
    toInsert->setBalance(0);
    if (parent->getBalance() != 0) insertFix(parent, toInsert);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n) {
  if (p == nullptr || p->getParent() == nullptr) return;

  AVLNode<Key, Value>* g = p->getParent();

  if (p == g->getLeft()) {
     g->setBalance(g->getBalance() - 1);
     if (g->getBalance() == 0) return;
     else if (g->getBalance() == -1) insertFix(g, p);
     else {
        if (p->getBalance() == -1) {
          rotateRight(g);
          g->setBalance(0);
          p->setBalance(0);
        }
      else {
        rotateLeft(p);
        rotateRight(g);
        if (n->getBalance() == -1) {
          p->setBalance(0);
          g->setBalance(1);
          n->setBalance(0);
        }
        else if (n->getBalance() == 0) {
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else if (n->getBalance() == 1) {
          p->setBalance(-1);
          g->setBalance(0);
          n->setBalance(0);
        }
      } 
    }
  }

  else {
    g->setBalance(g->getBalance() + 1);
    if (g->getBalance() == 0) return;

    else if (g->getBalance() == 1) {
      insertFix(g, p);
    }
    else {
      if (p->getBalance() == 1) {
        rotateLeft(g);
        g->setBalance(0);
        p->setBalance(0);
      }
      else {
        rotateRight(p);
        rotateLeft(g);
        if (n->getBalance() == 1) {
          p->setBalance(0);
          g->setBalance(-1);
          n->setBalance(0);
        } 
        else if (n->getBalance() == 0) {
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else if (n->getBalance() == -1) {
          p->setBalance(1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
  
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
  if (!node || !node->getLeft()) return;

  AVLNode<Key, Value>* child = node->getLeft();
  AVLNode<Key, Value>* grandchild = child->getRight();
  AVLNode<Key, Value>* parent = node->getParent();

  node->setLeft(grandchild);
  if (grandchild) grandchild->setParent(node);

  child->setRight(node);
  node->setParent(child);
  updateLinks(parent, node, child);

}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
  if (!node || !node->getRight()) return;

  AVLNode<Key, Value>* child = node->getRight();
  AVLNode<Key, Value>* grandchild = child->getLeft();
  AVLNode<Key, Value>* parent = node->getParent();
  node->setRight(grandchild);
  if (grandchild) grandchild->setParent(node);

  child->setLeft(node);
  node->setParent(child);
  updateLinks(parent, node, child);
}
/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
  AVLNode<Key, Value>* toRemove = internalFind(key);
  if (toRemove == nullptr) return;

  if (toRemove->getLeft() != nullptr && toRemove->getRight() != nullptr) {
    AVLNode<Key, Value>* pred = predecessor(toRemove);
    nodeSwap(toRemove, pred);
  }

  int diff = 0;
  AVLNode<Key, Value>* parent = toRemove->getParent();
  AVLNode<Key, Value>* childOfRemove;
  if (parent != nullptr) {
    if (toRemove == parent->getLeft()) {
      diff = 1;
    }
    else diff = -1;
  }
  if (toRemove->getRight() == nullptr) {
    childOfRemove = toRemove->getLeft();
  }
  else {
    childOfRemove = toRemove->getRight();
  }
  updateLinks(parent, toRemove, childOfRemove);
  delete toRemove;
  toRemove = nullptr;
  removeFix(parent, diff);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current) {
  if (current == nullptr) return nullptr;
    
    if (current->getLeft() != nullptr) {
      AVLNode<Key, Value>* pred = current->getLeft();
      while (pred->getRight() != nullptr) { 
        pred = pred->getRight();
      }
      return pred;
    }
    
    AVLNode<Key, Value>* parent = current->getParent();
    AVLNode<Key, Value>* child = current;
    
    while (parent != nullptr && child == parent->getLeft()) { 
      child = parent;
      parent = parent->getParent();
    }
    
    return parent;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::internalFind(const Key& key) const {
  AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (curr != nullptr) {
        if (key < curr->getKey()) {
            curr = curr->getLeft();
        }
        else if (key > curr->getKey()) {
            curr = curr->getRight();
        }
        else return curr; 
    }
    return nullptr; 
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff) {
  if (n == nullptr) return;

    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = 0; 
    if (p != nullptr) {
      ndiff = (n == p->getLeft()) ? 1 : -1;
    }

    if (diff == 1) {
      n->updateBalance(diff); 

      if (n->getBalance() == 0) {
        removeFix(p, ndiff);
      } 
      else if (n->getBalance() == 1) {
        return;
      } 
      else { 
        AVLNode<Key, Value>* c = n->getRight(); 
        int cBalance = c->getBalance();

        if (cBalance == 1) { 
          rotateLeft(n);
          n->setBalance(0); 
          c->setBalance(0); 
          removeFix(p, ndiff);
        } 
        else if (cBalance == 0) { 
          rotateLeft(n);
          n->setBalance(1); 
          c->setBalance(-1);
          return;
        } 
        else { 
          AVLNode<Key, Value>* g = c->getLeft();
          int gBalance = g->getBalance(); 
          rotateRight(c);
          rotateLeft(n);
          if (gBalance == 1) { n->setBalance(-1); c->setBalance(0); }
          else if (gBalance == 0) { n->setBalance(0); c->setBalance(0); }
          else { n->setBalance(0); c->setBalance(1); }
          g->setBalance(0); 
          removeFix(p, ndiff);
        }
      }
    }
    else if (diff == -1) {
      n->updateBalance(diff); 

      if (n->getBalance() == 0) {
        removeFix(p, ndiff);
      } 
      else if (n->getBalance() == -1) {
        return;
      } 
      else { 
        AVLNode<Key, Value>* c = n->getLeft(); 
        int cBalance = c->getBalance();

        if (cBalance == -1) { 
          rotateRight(n);
          n->setBalance(0); 
          c->setBalance(0); 
          removeFix(p, ndiff);
        } 
        else if (cBalance == 0) { 
          rotateRight(n);
          n->setBalance(-1); 
          c->setBalance(1); 
          return;
        }
        else { 
          AVLNode<Key, Value>* g = c->getRight();
          int gBalance = g->getBalance(); 
          rotateLeft(c);
          rotateRight(n);
          if (gBalance == -1) { n->setBalance(1); c->setBalance(0); }
          else if (gBalance == 0) { n->setBalance(0); c->setBalance(0); }
          else {  n->setBalance(0); c->setBalance(-1); }
          g->setBalance(0); 
          removeFix(p, ndiff);
        }
      }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
