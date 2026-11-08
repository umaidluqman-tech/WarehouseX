#ifndef PICKING_PLAN_H
#define PICKING_PLAN_H
#include "common.h"
#include "graph.h"

// One pickup: take qty of a product from one rack.
struct Stop {
    string rackId;
    string rackLocation;
    string productId;
    int qty;
    bool collected;     // set true after collection:
                        // prevents double deduction
    Stop* next;
};

// One leg = one route + the kg carried on it.
// weightCarried is 0 for an empty leg.
struct Leg {
    Route* route;       // owned, deleted in ~Trip
    double weightCarried;
    Leg* next;
};

struct Trip {
    int tripNo;
    string startLoc;        // where the robot starts this trip
    Stop* stops;            // pickups in visiting order; stops at
                            // the SAME rack are consecutive
    Leg* legs;              // start->rack1, rack1->rack2 ...
                            // lastRack->packing (ONE leg per
                            // distinct rack visit)
    string packingId;       // destination, e.g. L12
    int totalDistance;
    double totalWeight;     // kg
    double batteryUse;      // %
    bool done;              // trip executed
    Trip* next;
    Trip(int no);
    ~Trip();                // free stops, legs, routes
    NO_COPY(Trip)
};

class PickingPlan {
public:
    string orderId;
    string robotId;
    bool partial;           // true = built for the "reserve only
                            // what is available" option
    Trip* trips;            // linked list
    int tripCount;
    int totalDistance;
    double totalBattery;

    PickingPlan();
    ~PickingPlan();         // free trips
    NO_COPY(PickingPlan)
    void addTrip(Trip* t);  // append at END, tripCount++
    Trip* nextUndoneTrip() const;
    bool allTripsDone() const;
    void recomputeTotals(); // sum of the trips
    void display() const;   // shaped like the PDF example; a
                            // rack with several products prints
                            // ONE "Visit Rack" and several "Collect"
};

#endif
