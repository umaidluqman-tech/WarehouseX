#ifndef ORDER_SYSTEM_H
#define ORDER_SYSTEM_H
#include "order_queue.h"
#include "robot_list.h"      // Member 3
#include "plan_registry.h"   // Member 3

// Owns the main order list (all orders) + the pending queue.
class OrderSystem {
private:
    Order* head;
    OrderQueue queue;
    void clearAll();
public:
    OrderSystem();
    ~OrderSystem();            // delete every Order
    NO_COPY(OrderSystem)
    // 1. orders.txt: format O101,CU12,P01:5;P03:4
    //    Order id "O"+digits, customer "CU"+digits, every
    //    product must exist, qty > 0. A bad line is skipped as a
    //    whole (print why). An order with at least one valid
    //    item is enqueued in FILE ORDER.
    bool loadOrders(const string& file, const ProductTree& t);
    // 2. new order (unique id "O"+digits, customer "CU"+digits)
    //    -> status Pending, NOT queued yet (it has no items).
    bool addOrder(const string& orderId, const string& custId);
    // 3. add an item (call repeatedly for several products)
    bool addItemToOrder(const string& orderId,
                        const string& productId, int qty,
                        const ProductTree& t);
    // 4. search
    Order* findOrder(const string& orderId) const;
    // 5. enqueue at the REAR: order must be Pending, not already
    //    queued and have at least one item with remaining > 0
    //    (an empty order is rejected).
    bool enqueuePending(const string& orderId);
    //    Op 5 for the whole list: enqueue every Pending order
    //    that is eligible (in list order). Returns how many.
    int enqueueAllPending();
    // 6 / 7. display queue + peek + size
    void displayQueue() const;
    void peekFront() const;
    // 8. Cancel:
    //   Pending  -> reject if hasAnyDelivery(), else removeById
    //               from queue (if queued)
    //   Reserved/Assigned (no trip started: !hasAnyDelivery) ->
    //       tree.releaseOrderReservations, plans.removeByOrder,
    //       free the robot (robots.releaseRobot), clear
    //       assignedRobotId
    //   Partial/Completed/Cancelled -> reject
    //   then status = ORD_CANCELLED
    bool cancelOrder(const string& orderId, ProductTree& tree,
                     RobotList& robots, PlanRegistry& plans);
    // 9. Remove cancelled orders still sitting in the queue.
    int purgeCancelledFromQueue();
    // 10. status update helper: refuses a change that
    //     isStatusTransitionAllowed() does not allow.
    bool setStatus(const string& orderId, OrderStatus s);
    // 11. show completed, then pending
    void displayCompletedAndPending() const;
    // 12. is the ID already used? (duplicate rule)
    bool idExists(const string& orderId) const;
    // 13. (extra) finish an unfinished order: a Partially
    //     Fulfilled order with NO robot assigned and
    //     hasRemaining() becomes Pending and joins the REAR of
    //     the queue (deliveries stay delivered).
    bool requeueRemainder(const string& orderId);
    // Is this product inside any order that is not Completed
    // or Cancelled? (menu uses it to refuse product deletion)
    bool productInUse(const string& productId) const;
    // ---- API for Member 3 ----
    OrderQueue& getQueue() { return queue; }
};

#endif
