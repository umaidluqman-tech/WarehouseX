#ifndef RACK_LIST_H
#define RACK_LIST_H
#include "common.h"

// One reservation made by one order on one rack.
struct ReservationNode {
    string orderId;
    int qty;
    ReservationNode* next;
};

// One rack holding a product (node in a product's list).
// INVARIANT: sum of reservations' qty == reserved.
struct RackEntry {
    string rackId;
    int total;
    int reserved;
    ReservationNode* reservations;
    RackEntry* next;
    int available() const { return total - reserved; }
};

// ---- Helper functions working on ONE rack entry ----
// Add 'qty' to the reservation of orderId (merge if the order
// already has a record), reserved += qty.
// Fails if qty <= 0 or qty > available().
bool rackReserve(RackEntry* r, const string& orderId, int qty);
// Remove ALL reservation records of orderId from r;
// reserved -= their sum. Returns released qty (0 if none).
int rackReleaseOrder(RackEntry* r, const string& orderId);
// Collection (Module 6): total -= qty AND reserved -= qty AND
// reduce orderId's record by qty (delete the node if it hits
// 0). Fails if the order's record < qty. Nothing changes on
// failure.
bool rackCollect(RackEntry* r, const string& orderId, int qty);
// Free all reservation nodes of r (set pointer to nullptr).
void rackFreeReservations(RackEntry* r);
// Free a whole RackEntry list (nodes + reservation lists).
void freeRackList(RackEntry* head);

// Result of the allocation rule: list of (rack, quantity).
struct AllocationNode {
    string rackId;
    int qty;
    AllocationNode* next;
};
void freeAllocation(AllocationNode* head);

#endif
