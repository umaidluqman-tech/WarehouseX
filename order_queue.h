#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H
#include "order.h"

// Queue node points to the Order (the order itself lives in
// the main list; do NOT delete it when dequeuing).
struct QueueNode { Order* order; QueueNode* next; };

// FCFS queue as a linked list with front and rear pointers.
class OrderQueue {
private:
    QueueNode* front;
    QueueNode* rear;
    int count;
public:
    OrderQueue();
    ~OrderQueue();                      // delete nodes only
    NO_COPY(OrderQueue)
    void enqueue(Order* o);             // add at rear
    // put at FRONT (used by Cancel Assignment, Module 6)
    void enqueueFront(Order* o);
    Order* dequeue();                   // remove front, or null
    Order* peek() const;                // front, no removal
    // O(n) scan: remove this order from anywhere. If the removed
    // node was the LAST one, rear must move to its predecessor
    // (and front/rear become null when the queue gets empty).
    bool removeById(const string& orderId);
    bool contains(const string& orderId) const;
    int  size() const;
    bool isEmpty() const;
    // for Module 5: walk the queue WITHOUT removing
    // (skipped orders keep their positions)
    QueueNode* frontNode() const;
    // display front -> rear, no removal
    void display() const;
};

#endif
