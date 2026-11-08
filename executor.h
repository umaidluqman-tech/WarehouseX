#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "picking_plan.h"
#include "plan_builder.h"
#include "plan_registry.h"

class Executor {
private:
    Graph& graph;
    ProductTree& tree;
    RackRegistry& racks;
    OrderSystem& orders;
    RobotList& robots;
    PlanRegistry& plans;

    // Validation ONLY, changes nothing (validate first, then
    // apply). Checks, in order:
    //  0 order is not Cancelled and robot's task == plan order
    //  1 every leg's route still OPEN  (graph.isRouteOpen)
    //  2 battery >= trip battery + 10
    //  3 trip weight <= robot capacity
    //  4 reserved stock for each uncollected stop still exists
    //  5 robot is still at the trip start location
    // Writes the reason to 'reason'.
    bool validateTrip(const PickingPlan* p, const Trip* t,
                      const Robot* r, string& reason);
    // Apply ONE validated trip:
    //  for each stop (skip collected ones): tree.collectStock,
    //  stop->collected=true; robot battery -= trip battery;
    //  robot location = packing station; for each stop add qty
    //  to OrderItem.delivered; trip->done=true.
    void applyTrip(PickingPlan* p, Trip* t, Robot* r, Order* o);
    // After a trip: Completed if all items delivered, else
    // Partially Fulfilled. After the FINAL trip: release the
    // robot, clear assignedRobotId and remove the plan from the
    // registry (it is deleted; the caller's pointer becomes
    // nullptr). A partial plan ends as Partially Fulfilled: the
    // order menu's "re-queue unfinished order" can continue it.
    void finishTripStatus(PickingPlan*& p, Order* o);
public:
    Executor(Graph& g, ProductTree& t, RackRegistry& r,
             OrderSystem& os, RobotList& rl, PlanRegistry& pr);
    NO_COPY(Executor)
    // Op 1: show robot and plan.
    void displayPlan(const PickingPlan* p) const;
    // Menu 1: execute the next undone trip (validate -> apply).
    // p becomes nullptr when the last trip finished.
    bool executeTrip(PickingPlan*& p);
    // Menu 2: re-run Dijkstra between the same stops for EVERY
    // undone trip and replace the legs/trip totals. If a planned
    // rack OR the packing station is unreachable, report it and
    // offer Cancel Assignment. The stops and packing stations of
    // the plan are kept (document this).
    bool recalculateRoute(PickingPlan* p);
    // Menu 3: release un-collected reservations, free the
    // robot, order -> Pending and enqueueFront. Delivered
    // items stay delivered. Removes (deletes) the plan from the
    // registry; p becomes nullptr. The next plan for this order
    // uses remaining() quantities only.
    bool cancelAssignment(PickingPlan*& p);
    // Menu 4 ("assign but not execute now"): just return,
    // the plan stays in the PlanRegistry.
    // The execute menu loop: prints the box from the PDF
    // example and offers options 1-4.
    void runExecuteMenu(PickingPlan*& p);
};

#endif
