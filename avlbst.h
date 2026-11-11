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
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
};

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
  AVLNode<Key, Value>* rightChild = node->getRight();

  if(rightChild == nullptr){
    return;
  }

  AVLNode<Key, Value>* parent = node->getParent();

  // Move rightChild's left subtree
  node->setRight(rightChild->getLeft());

  if(rightChild->getLeft() != nullptr){
    rightChild->getLeft()->setParent(node);
  }

  // Connect rightChild to node's original parent
  rightChild->setParent(parent);

  if(parent == nullptr){
    this->root_ = rightChild;
  }else if(parent->getLeft() == node){
    parent->setLeft(rightChild);
  }else{
    parent->setRight(rightChild);
  }

  // Move node underneath rightChild
  rightChild->setLeft(node);
  node->setParent(rightChild);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
  AVLNode<Key, Value>* leftChild = node->getLeft();

  if(leftChild == nullptr){
    return;
  }

  AVLNode<Key, Value>* parent = node->getParent();

  // Move leftChild's right subtree
  node->setLeft(leftChild->getRight());

  if(leftChild->getRight() != nullptr){
    leftChild->getRight()->setParent(node);
  }

  // Connect leftChild to node's original parent
  leftChild->setParent(parent);

  if(parent == nullptr){
    this->root_ = leftChild;
  }else if(parent->getLeft() == node){
    parent->setLeft(leftChild);
  }else{
    parent->setRight(leftChild);
  }

  // Move node underneath leftChild
  leftChild->setRight(node);
  node->setParent(leftChild);
}


/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

  // Case 1: Empty tree
  if(this->root_ == nullptr){
    this->root_ = new AVLNode<Key, Value>(
    new_item.first, new_item.second, nullptr);
    return;
  }

  AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);

  AVLNode<Key, Value>* parent = nullptr;

  // Find the insertion location
  while(current != nullptr){
    parent = current;

    if(new_item.first < current->getKey()){
      current = current->getLeft();
    }else if(new_item.first > current->getKey()){
      current = current->getRight();
    }else{
      // Duplicate key: update the value
      current->setValue(new_item.second);
      return;
    }
  }

  AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>( new_item.first, new_item.second, parent);

  if(new_item.first < parent->getKey()){
    parent->setLeft(newNode);
    parent->updateBalance(-1);
  }else{
    parent->setRight(newNode);
    parent->updateBalance(1);
  }

  // Rebalance if necessary
  if(parent->getBalance() != 0){
    insertFix(parent, newNode);
  }
}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFix( AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
  if(parent == nullptr || parent->getParent() == nullptr){
    return;
  }

  AVLNode<Key, Value>* grandparent = parent->getParent();

  if(grandparent->getLeft() == parent){
    grandparent->updateBalance(-1);
  }else{
    grandparent->updateBalance(1);
  }

  if(grandparent->getBalance() == 0){
    return;
  }

  if(grandparent->getBalance() == -1 || grandparent->getBalance() == 1){
    insertFix(grandparent, parent);
    return;
  }

  // Left-heavy subtree
  if(grandparent->getBalance() == -2){
    if (parent->getBalance() == -1){
      // Left-Left
      rotateRight(grandparent);
      parent->setBalance(0);
      grandparent->setBalance(0);
    }else{
      // Left-Right
      int balance = node->getBalance();

      rotateLeft(parent);
      rotateRight(grandparent);

      if(balance == -1){
        parent->setBalance(0);
        grandparent->setBalance(1);
      }else if(balance == 0){
        parent->setBalance(0);
        grandparent->setBalance(0);
      }else{
        parent->setBalance(-1);
        grandparent->setBalance(0);
      }

      node->setBalance(0);
    }
  }// Right-heavy subtree
  else if(grandparent->getBalance() == 2){
    if (parent->getBalance() == 1){
      // Right-Right
      rotateLeft(grandparent);
      parent->setBalance(0);
      grandparent->setBalance(0);
    }else{
      // Right-Left
      int balance = node->getBalance();
      rotateRight(parent);
      rotateLeft(grandparent);

      if(balance == 1){
        parent->setBalance(0);
        grandparent->setBalance(-1);
      }else if(balance == 0){
        parent->setBalance(0);
        grandparent->setBalance(0);
      }else{
        parent->setBalance(1);
        grandparent->setBalance(0);
      }

      node->setBalance(0);
    }
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
  AVLNode<Key, Value>* target = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

  if(target == nullptr){
    return;
  }

  // If target has two children, swap with predecessor
  if(target->getLeft() != nullptr && target->getRight() != nullptr){
    AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(target));
    nodeSwap(target, pred);
  }

  // Target now has at most one child
  AVLNode<Key, Value>* child =
    (target->getLeft() != nullptr)
    ? target->getLeft()
    : target->getRight();

  AVLNode<Key, Value>* parent = target->getParent();

  int diff = 0;

  if(parent != nullptr){
    if(parent->getLeft() == target){
      diff = 1;
    }else{
      diff = -1;
    }
  }

  if(child != nullptr){
    child->setParent(parent);
  }

  if(parent == nullptr){
    this->root_ = child;
  }else if(diff == 1){
    parent->setLeft(child);
  }else{
    parent->setRight(child);
  }

  delete target;

  if(parent != nullptr){
    removeFix(parent, diff);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(
    AVLNode<Key, Value>* node, int diff)
{
  while(node != nullptr){
    int balance = node->getBalance() + diff;

    AVLNode<Key, Value>* parent = node->getParent();

    int nextDiff = 0;
    if(parent != nullptr){
      nextDiff = (parent->getLeft() == node) ? 1 : -1;
    }

    if(balance == -1 || balance == 1){
      node->setBalance(balance);
      return;
    }

    if(balance == 0){
      node->setBalance(0);
      node = parent;
      diff = nextDiff;
      continue;
    }

    if(balance == -2){
      AVLNode<Key, Value>* left = node->getLeft();
      int leftBalance = left->getBalance();

      if(leftBalance <= 0){
        rotateRight(node);

        if(leftBalance == 0){
          node->setBalance(-1);
          left->setBalance(1);
          return;
        }

        node->setBalance(0);
        left->setBalance(0);
      }else{
        AVLNode<Key, Value>* middle = left->getRight();
        int middleBalance = middle->getBalance();

        rotateLeft(left);
        rotateRight(node);

        if(middleBalance == -1){
          node->setBalance(1);
          left->setBalance(0);
        }else if(middleBalance == 0){
          node->setBalance(0);
          left->setBalance(0);
        }else{
          node->setBalance(0);
          left->setBalance(-1);
        }

        middle->setBalance(0);
      }
    }else if (balance == 2){
      AVLNode<Key, Value>* right = node->getRight();
      int rightBalance = right->getBalance();

      if(rightBalance >= 0){
        rotateLeft(node);

        if(rightBalance == 0){
          node->setBalance(1);
          right->setBalance(-1);
          return;
        }

        node->setBalance(0);
        right->setBalance(0);
      }else{
        AVLNode<Key, Value>* middle = right->getLeft();
        int middleBalance = middle->getBalance();

        rotateRight(right);
        rotateLeft(node);

        if(middleBalance == 1){
          node->setBalance(-1);
          right->setBalance(0);
        }else if(middleBalance == 0){
          node->setBalance(0);
          right->setBalance(0);
        }else{
          node->setBalance(0);
          right->setBalance(1);
        }

        middle->setBalance(0);
      }
    }

        node = parent;
        diff = nextDiff;
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
