#ifndef MENUS_M2_H
#define MENUS_M2_H
#include "product_tree.h"
#include "order_system.h"
#include "undo_stack.h"

// All 11 Module 2 operations; the manual quantity update must
// call undo.recordQtyUpdate(product, rack, old, new). Deleting a
// product is refused when os.productInUse(id) is true.
void runInventoryMenu(ProductTree& t, RackRegistry& reg,
                      UndoManager& undo, OrderSystem& os);
// All 12 Module 3 operations + "re-queue unfinished order".
// "Add new order" asks for items in a loop and enqueues the
// order when the user finishes with at least one item.
void runOrderMenu(OrderSystem& os, ProductTree& t,
                  RobotList& robots, PlanRegistry& plans);

#endif
