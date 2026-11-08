#ifndef PLAN_BUILDER_H
#define PLAN_BUILDER_H
#include "picking_plan.h"
#include "product_tree.h"
#include "order_system.h"
#include "robot_list.h"
#include "rack_registry.h"
#include "plan_registry.h"

// What was planned for ONE rack (before trips are made).
struct PlannedPick {
    string productId;
    string rackId;
    string rackLocation;
    int qty;
    double unitWeight;
    bool used;           // already put in a trip stop
    PlannedPick* next;
};

class PlanBuilder {
private:
    Graph& graph;
    ProductTree& tree;
    RackRegistry& racks;

    // ---- helpers (each is one small step of the heuristic)
    // Do all items have availableStock >= remaining()?
    bool hasFullStock(const Order* o, string& reason) const;
    // Step 1: for each order item with remaining() > 0 call
    //   tree.allocateStock(product, remaining(), robotLoc ...).
    //   Build the list of picks.
    //   allowPartial=false: any shortfall -> reason, nullptr.
    //   allowPartial=true : take what is available (shortfall is
    //   ignored); nullptr only if NOTHING can be picked.
    //   A product missing from the tree -> reason, nullptr.
    PlannedPick* allocatePicks(const Order* o,
                               const string& robotLoc,
                               bool allowPartial,
                               string& reason);
    void freePicks(PlannedPick* head);
    // Step 2: nearest-reachable-rack order. From 'current'
    //   pick the unused pick whose rackLocation is nearest
    //   (Dijkstra); tie -> smaller RackID (compareIds), then
    //   smaller ProductID. Return null if none.
    PlannedPick* nextNearestPick(PlannedPick* head,
                                 const string& current);
    // Step 3: build ONE trip. Take the nearest rack, then ALL
    //   unused picks at that SAME rack (in ProductID order)
    //   before moving on, so a rack is visited once. Add units
    //   until capacity is reached; a pick may be SPLIT by whole
    //   units across trips:
    //     unitsFit = (int)floor((capacityLeft + EPS)/unitWeight)
    //   If the trip is still EMPTY and zero units fit -> fail
    //   (reason = capacity) so the loop can never run forever.
    // Step 3b: when a trip closes -> nearest packing station
    //   (graph.findNearestOfType) -> next trip starts there.
    // Returns nullptr + reason on failure.
    Trip* buildNextTrip(PlannedPick* picks, Robot* robot,
                        const string& startLoc, int tripNo,
                        string& endLocOut, string& reason);
    // Step 4/5: for a trip add the legs and compute distance,
    //   weight and battery leg by leg:
    //   battery += legConsumption(dist, rate, carried, cap)
    //   'carried' grows after each pickup. One leg per distinct
    //   rack; the last leg carries the whole trip weight to the
    //   packing station. false if any route does not exist.
    bool fillTripLegs(Trip* t, Robot* robot,
                      const string& startLoc);
    // Feasible only if battery >= plan battery + 10.
    bool isFeasible(const PickingPlan* p, const Robot* r) const;
    // compare two plans: fewer trips -> smaller distance ->
    // smaller RobotID (compareIds). true if a is better than b.
    bool isBetter(const PickingPlan* a,
                  const PickingPlan* b) const;

public:
    PlanBuilder(Graph& g, ProductTree& t, RackRegistry& r);
    NO_COPY(PlanBuilder)

    // Build a plan for ONE robot. nullptr + reason on failure
    // (capacity, unreachable rack, ...). The plan is NOT
    // feasibility-filtered here.
    PickingPlan* buildPlanForRobot(const Order* o, Robot* r,
                                   bool allowPartial,
                                   string& reason);
    // Robot Selection Rule: loop robots with
    //   status Available && isEligible(o->remainingWeight,
    //   o->heaviestRemainingUnit) && canCarryUnit(heaviest)
    //   build a plan, drop infeasible ones, keep the best by
    //   isBetter.
    // If none: set 'reason' (no available robot / eligibility /
    //   capacity / battery -> "suggest charging RBxx").
    PickingPlan* selectBestPlan(const Order* o,
                                RobotList& robots,
                                bool allowPartial,
                                string& reason);
    // Op 1: walk the queue FRONT to REAR (frontNode, no
    //   dequeue). For each order:
    //   - empty order or product missing -> print reason, skip
    //   - stock shortage -> print reason, remember the FIRST such
    //     order in shortageOrderOut, skip (it keeps its position)
    //   - stock OK -> selectBestPlan(..., false, ...). On success
    //     return the order + plan. On failure (robot/battery/
    //     eligibility/capacity) print the reason, put it into
    //     reasonOut and STOP: return nullptr. Strict FCFS: only a
    //     SHORTAGE lets later orders overtake.
    Order* findNextProcessableOrder(OrderSystem& os,
                                    RobotList& robots,
                                    PickingPlan*& planOut,
                                    Order*& shortageOrderOut,
                                    string& reasonOut);
    // The user answered YES to "reserve only the available
    // quantity?": plan for the shortage order with
    // allowPartial=true (plan->partial = true).
    PickingPlan* planPartial(Order* o, RobotList& robots,
                             string& reason);
    // Op 9: ALL-OR-NOTHING reservation after the user confirms:
    //   for every stop call tree.reserveStock; if any fails
    //   call tree.releaseOrderReservations(orderId) and return
    //   false (nothing stays reserved).
    bool reservePlan(const PickingPlan* p);
    // Op 10: reservePlan -> order Reserved -> robots.assignRobot
    //   (if it fails: release the reservations, order back to
    //   Pending, return false) -> order Assigned, remove the
    //   order from the queue, set order.assignedRobotId and hand
    //   the plan to 'plans' (plans.add). On true the caller must
    //   NOT delete p.
    bool confirmAssignment(Order* o, PickingPlan* p,
                           RobotList& robots, OrderSystem& os,
                           PlanRegistry& plans);
};

#endif
