#ifndef MENUS_M1_H
#define MENUS_M1_H
#include "graph.h"
#include "charging.h"
#include "undo_stack.h"

// Each function shows a loop menu and validates every input
// (use readIntInRange / readId from utils.h).

// Graph menu: all 10 Module 1 operations (block and remove
// are SEPARATE options; a successful block calls
// undo.recordBlockPath). Route option: check locationExists for
// BOTH ids first and say which one is missing; a nullptr route
// then means "no route exists".
void runGraphMenu(Graph& g, UndoManager& undo);
// Charging menu: all 10 Module 7 operations.
void runChargingMenu(Graph& g, ChargerManager& cm,
                     RobotList& robots);
// Undo menu: show last / undo / discard.
void runUndoMenu(UndoManager& um, ProductTree& t,
                 Graph& g, RobotList& r);

#endif
