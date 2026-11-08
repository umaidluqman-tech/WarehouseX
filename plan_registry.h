#ifndef PLAN_REGISTRY_H
#define PLAN_REGISTRY_H
#include "picking_plan.h"

// Owns every assigned picking plan (executed or not yet).
// One plan per order. Lives in main(), so plans survive menu
// changes and are freed exactly once at program end.
struct PlanNode { PickingPlan* plan; PlanNode* next; };

class PlanRegistry {
private:
    PlanNode* head;
    int count;
public:
    PlanRegistry();
    ~PlanRegistry();                 // delete every plan + node
    NO_COPY(PlanRegistry)
    // Take ownership of p (reject nullptr or a second plan for
    // the same order id).
    bool add(PickingPlan* p);
    PickingPlan* findByOrder(const string& orderId) const;
    // Unlink AND delete the plan of this order. false if none.
    // Used by: finished execution, Cancel Assignment and
    // OrderSystem::cancelOrder.
    bool removeByOrder(const string& orderId);
    int  size() const;
    PlanNode* firstNode() const;     // to walk the list
    // list "order, robot, trips done/total" for the menu
    void displayAll() const;
};

#endif
