#ifndef ORDER_H
#define ORDER_H
#include "common.h"
#include "product_tree.h"

struct OrderItem {
    string productId;
    int qty;            // ordered
    int delivered;      // delivered so far (starts at 0)
    OrderItem* next;
    // quantity still to be delivered (planners use THIS, never qty)
    int remaining() const { return qty - delivered; }
};

// Allowed status changes (used by OrderSystem::setStatus):
//   Pending   -> Reserved, Cancelled
//   Reserved  -> Assigned, Pending, Cancelled
//   Assigned  -> Partial, Completed, Pending, Cancelled
//   Partial   -> Completed, Pending      (never Cancelled)
//   Completed / Cancelled -> nothing (final)
bool isStatusTransitionAllowed(OrderStatus from, OrderStatus to);

class Order {
public:
    string id;              // O101
    string customerId;      // CU12
    OrderStatus status;
    OrderItem* items;       // linked list
    string assignedRobotId; // "" if none
    Order* next;            // next in the main order list

    Order(const string& id, const string& cust);
    ~Order();               // free the item list
    NO_COPY(Order)
    // Add item: product must exist in tree, qty > 0.
    // If the product is already in this order -> MERGE
    // (qty += qty). Only allowed when status == ORD_PENDING.
    bool addItem(const string& productId, int qty,
                 const ProductTree& tree);
    OrderItem* findItem(const string& productId) const;
    // sum(qty * unitWeight) - needs the tree for weights.
    // A product missing from the tree counts as weight 0.
    double totalWeight(const ProductTree& tree) const;
    // sum(remaining * unitWeight): what a robot still has to carry
    double remainingWeight(const ProductTree& tree) const;
    // largest single unit weight among items with remaining > 0
    double heaviestRemainingUnit(const ProductTree& tree) const;
    // number of items (lines) in the order
    int itemCount() const;
    // at least one item still has remaining() > 0
    bool hasRemaining() const;
    // delivered == qty for EVERY item
    bool isFullyDelivered() const;
    // any delivered > 0
    bool hasAnyDelivery() const;
    void display() const;
};

#endif
