#ifndef MENUS_M3_H
#define MENUS_M3_H
#include "robot_list.h"
#include "plan_builder.h"
#include "executor.h"
#include "undo_stack.h"
#include "charging.h"

// Robot menu (10 ops of Module 4). The maintenance change:
//  - if the robot is Waiting for Charging, first call
//    cm.removeFromQueue (status becomes Available), then
//  - call setMaintenance and, on success, call
//    undo.recordRobotMaintenance(id, oldStatus).
void runRobotMenu(RobotList& r, Graph& g, UndoManager& undo,
                  ChargerManager& cm);
// Planning + execution menu:
//  1 find next order -> show plan -> confirm? -> (reserve+assign)
//    -> run execute menu or "assign but not execute now"
//  2 if the front orders only failed on a SHORTAGE: ask the
//    user "reserve only the available quantity?" and, on yes,
//    use pb.planPartial
//  3 execute an already assigned plan (choose from 'plans')
//  4 list assigned plans
void runPlanningMenu(PlanBuilder& pb, Executor& ex,
                     OrderSystem& os, RobotList& rl,
                     PlanRegistry& plans);

#endif
