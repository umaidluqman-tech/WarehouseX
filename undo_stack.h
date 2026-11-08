#ifndef UNDO_STACK_H
#define UNDO_STACK_H
#include "common.h"
#include "graph.h"
#include "product_tree.h"   // Member 2
#include "robot_list.h"     // Member 3

enum OpType { OP_QTY_UPDATE, OP_BLOCK_PATH, OP_ROBOT_MAINT };

// One history record. A single struct with all possible
// fields; only the fields of its 'type' are used.
struct OpRecord {
    OpType type;
    // OP_QTY_UPDATE
    string productId, rackId;
    int oldQty, newQty;
    // OP_BLOCK_PATH  (old status is always OPEN, because
    // blockPathway refuses an already-blocked pathway)
    string locA, locB;
    // OP_ROBOT_MAINT  (old status is Available or Unavailable
    // only; the menu never records Waiting/Charging)
    string robotId;
    RobotStatus oldRobotStatus;
    OpRecord* next;   // node below in the stack
};

// Linked-list stack (push/pop at the top).
class UndoStack {
private:
    OpRecord* top;
    int size;
public:
    UndoStack();
    ~UndoStack();              // delete all records
    NO_COPY(UndoStack)
    void push(OpRecord* rec);  // rec->next = top; top = rec
    OpRecord* peek() const;    // top or nullptr
    bool pop();                // delete top. false if empty
    bool isEmpty() const;
    int  getSize() const;
};

class UndoManager {
private:
    UndoStack stack;
public:
    NO_COPY(UndoManager)
    UndoManager() {}
    // ---- Op 1: record (call right after the real operation
    // succeeded) ----
    void recordQtyUpdate(const string& productId,
                         const string& rackId,
                         int oldQty, int newQty);
    // Call only after blockPathway() returned true.
    void recordBlockPath(const string& a, const string& b);
    void recordRobotMaintenance(const string& robotId,
                                RobotStatus oldStatus);
    // ---- Op 2: print the top record in readable form ----
    void displayLast() const;
    // ---- Op 3,4,5: undo ----
    // Steps: peek top -> check rejection rule -> if OK restore
    // exactly and pop -> report. If rejected: print the reason,
    // DO NOT pop, DO NOT change anything. Rules (PDF table):
    //   QTY  : tree.setQuantityChecked(product, rack, oldQty,
    //          reason) fails -> rejected, show 'reason'
    //   BLOCK: NEVER rejected. If the pathway is still BLOCKED
    //          -> reopen it. If it was removed or is already
    //          OPEN, print "nothing to restore" and pop.
    //   MAINT: rejected if the robot is missing, has an active
    //          task, is Charging or Waiting; otherwise
    //          setStatus(oldRobotStatus).
    // Returns true only if the undo was applied.
    bool undoLast(ProductTree& tree, Graph& graph,
                  RobotList& robots);
    // "A menu option may discard the top entry."
    bool discardTop();
};

#endif
