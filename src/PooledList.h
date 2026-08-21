#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include  "Order.h"

/*
Comments for learning

Node someNode;          // an actual Node object, sitting somewhere in memory
Node* ptr = &someNode;  // ptr holds someNode's address — just a number, like0x1000

ptr->order    // means: go to address0x1000, find the Node living there, give me its `order` field

& goes from object → address. * goes from address → object.

So Node* node altogether means: "create a variable named node, and its type is Node* (an address-holder for a Node).
*/

struct Node {
    Order order;
    Node* next = nullptr;
    Node* prev = nullptr;  
};

class NodePool {
    private:
        std::vector<Node> pool; // our one million * n bytes 
        std::vector<size_t> freeIndicesList;
        size_t nextFreeIndex = 0; //init
    
    public:
        explicit NodePool(size_t capacity) :pool(capacity) {}
 
        Node* allocate() {
            if (!freeIndicesList.empty()) {
                size_t index = freeIndicesList.back();
                freeIndicesList.pop_back();
                return &pool[index]; // means "give me the memory address of the object living at pool[index]," returned as a pointer (Node*). & means the opposite operation — "take this real object, and tell me its address." Since pool[index] is a real
            } else {
                size_t index = nextFreeIndex;
                nextFreeIndex++;
                return &pool[index];
            }
        }

        void deallocate(Node* node) {
            size_t index = node - pool.data();
            freeIndicesList.push_back(index);
        }

};

class PriceLevelList {
    private:
        NodePool& pool; // ref to the actual pool
        Node* head = nullptr; //store an addy using (*) not the node struct itself
        Node* tail = nullptr;
    
    public:
        explicit PriceLevelList(NodePool& sharedPool) : pool(sharedPool) {}
            /*
            Python3: 
            class PriceLevelList:
                def __init__(self, shared_pool):
                    self.pool = shared_pool
            */
        Node* push_back(const Order& order) {
            Node* node = pool.allocate();
            node->order = order; //basically node.order = order but node is a pointer that holds an addy
            node->next = nullptr;
            node->prev = tail;

            if (tail == nullptr) {
                head = node;
            } else {
                tail->next = node;
            }
            tail = node;

            return node;
        }

        void erase(Node* node) {
            
            if (node == head && node == tail) {
                head = nullptr;
                tail = nullptr;
            } else if (node == head) {
                node->next->prev = nullptr;
                head = node->next;
            } else if (node == tail) {
                node->prev->next = nullptr;
                tail = node->prev;
            } else {
            node->next->prev = node->prev;
            node->prev->next = node->next;
            }

            pool.deallocate(node);

        }

        bool empty() const {
            if (head == nullptr) {
                return true;
            } else {
                return false;
            }
        }

        Node* front() {
            return head;
        }
};


/*
 * Node* allocate()  -> returns the ADDRESS of an already-existing Node.
 *                      Doesn't create/copy/contain a Node — just says "here's where one lives."
 *
 * struct Node { }   -> defines the REAL shape/blueprint (no star — you can't have "address of X" contain fields).
 * Node*             -> a variable that HOLDS an address to a Node. Only makes sense once Node is defined.
 *
 * std::vector<Node> pool -> the ONE place real Node objects physically exist.
 *                           Everything else (head, tail, next, prev, allocate()'s return)
 *                           is just an address pointing back into this vector.
 */