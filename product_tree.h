#ifndef PRODUCT_TREE_H
#define PRODUCT_TREE_H
#include "common.h"
#include "rack_list.h"
#include "graph.h"
#include "rack_registry.h"

// One AVL node = one unique product.
struct ProductNode {
    string id;
    string name;
    double unitWeight;     // kg
    int reorderLevel;
    RackEntry* racks;      // linked list of racks holding it
    ProductNode* left;
    ProductNode* right;
    int height;            // leaf = 1, null = 0
};

class ProductTree {
private:
    ProductNode* root;

    // ---------- AVL helpers (compare ids with compareIds(), so
    // P200 < P1000) ----------
    int  heightOf(ProductNode* n) const;          // null -> 0
    void updateHeight(ProductNode* n);            // 1+max(l,r)
    int  balanceFactor(ProductNode* n) const;     // h(l)-h(r)
    ProductNode* rotateRight(ProductNode* y);     // LL case
    ProductNode* rotateLeft(ProductNode* x);      // RR case
    // Fix imbalance at n: handles LL, LR, RR, RL.
    // Return the new root of this subtree.
    ProductNode* rebalance(ProductNode* n);
    // Recursive insert; sets 'inserted' false on duplicate.
    ProductNode* insertNode(ProductNode* n, ProductNode* fresh,
                            bool& inserted);
    // Recursive delete (3 cases: leaf, 1 child, 2 children ->
    // use the smallest node of the right subtree). Free the
    // removed node's rack list. Rebalance on the way back up.
    ProductNode* deleteNode(ProductNode* n, const string& id);
    ProductNode* minNode(ProductNode* n) const;
    ProductNode* searchNode(ProductNode* n,
                            const string& id) const;
    // Post-order delete of the whole tree (destructor).
    void destroy(ProductNode* n);
    // Recursive helpers for reports
    int  sumTotal(RackEntry* r) const;       // sum of total
    int  sumAvailable(RackEntry* r) const;   // sum of available
    void lowStockRec(ProductNode* n) const;  // in-order walk
    void printTreeRec(ProductNode* n, int depth) const;
    bool anyReservation(RackEntry* r) const; // reserved > 0 ?
    void releaseOrderRec(ProductNode* n, const string& orderId,
                         int& releasedTotal);
    RackEntry* findRack(ProductNode* p,
                        const string& rackId) const;

public:
    ProductTree();
    ~ProductTree();
    NO_COPY(ProductTree)

    // ---------- loading ----------
    // products.txt (insertProduct per line) then inventory.txt
    // (addRackEntry per line; reserved starts at 0).
    // Print why for every rejected line.
    bool loadProducts(const string& file);
    bool loadInventory(const string& file,
                       const RackRegistry& racks);

    // ---------- Module 2 operations ----------
    // 1. Insert; reject duplicate id, id not "P"+digits,
    //    weight<=0, reorder<0.
    bool insertProduct(const string& id, const string& name,
                       double weight, int reorder);
    // 2 + 11. Delete; reject if missing, or any rack has
    // reserved > 0 (print the reason). (The menu ALSO refuses
    // when OrderSystem::productInUse() is true.)
    bool deleteProduct(const string& id);
    // 3. Search (returns node or nullptr).
    ProductNode* searchProduct(const string& id) const;
    // 4. Add rack entry (rack must be in racks.txt - use the
    // registry; reject duplicate rack for this product;
    // quantity >= 0).
    bool addRackEntry(const string& productId,
                      const string& rackId, int qty,
                      const RackRegistry& racks);
    // 5. Manual admin update. New total must be >= 0 AND
    // >= reserved. Returns old qty through oldQtyOut (for
    // undo).
    bool updateQuantity(const string& productId,
                        const string& rackId, int newTotal,
                        int& oldQtyOut);
    // Same checks but used by Module 8 undo. Returns false and
    // fills 'reason' (e.g. "rack entry no longer exists" or
    // "restored quantity 5 is below reserved 8") so the undo
    // menu can SHOW the reason. Changes nothing on failure.
    bool setQuantityChecked(const string& productId,
                            const string& rackId, int newTotal,
                            string& reason);
    // 6. Remove rack entry only if total==0 and reserved==0.
    bool removeRackEntry(const string& productId,
                         const string& rackId);
    // 7. Print racks: total, reserved, available.
    void displayRacks(const string& productId) const;
    // 8 / 9. Stock over all racks. -1 if product missing.
    int totalStock(const string& productId) const;
    int availableStock(const string& productId) const;
    // 10. Products with availableStock < reorderLevel.
    void displayLowStock() const;
    // Print every product (in-order) for the menu / debugging.
    void displayAllProducts() const;
    // Print the tree sideways with heights (screenshot proof
    // that the AVL tree stays balanced).
    void displayTreeShape() const;

    // ---------- API used by Module 3, 5, 6 ----------
    // Rack allocation rule (PDF Module 2). Does NOT change
    // data. refLocation = robot's location (the reference used
    // for the whole order: the plan is allocated BEFORE the stops
    // are ordered; document this in the report).
    // Algorithm:
    //   remaining = qty
    //   loop while remaining > 0:
    //     candidates = racks with available>0 and
    //       graph.shortestDistance(refLocation, rackLoc) != -1
    //       (rack location via the registry), not used yet
    //     if none -> break (shortfall remains)
    //     A) if any candidate has available >= remaining:
    //          choose nearest of those; tie -> larger
    //          available, then smaller RackID (compareIds);
    //          take remaining
    //     B) else choose nearest candidate (same ties) and take
    //          ALL its available
    //     remaining -= taken (never reuse the same rack twice)
    //   shortfallOut = remaining
    // Returns the allocation list (caller frees it with
    // freeAllocation) or nullptr if nothing was allocated.
    // Callers pass qty = the REMAINING quantity of an item
    // (ordered - delivered), never the full ordered quantity.
    AllocationNode* allocateStock(const string& productId,
                                  int qty,
                                  const string& refLocation,
                                  Graph& g,
                                  const RackRegistry& reg,
                                  int& shortfallOut);
    // Reserve qty at a rack for an order (uses rackReserve).
    bool reserveStock(const string& productId,
                      const string& rackId, int qty,
                      const string& orderId);
    // Release every reservation of an order in the WHOLE tree
    // (used by cancel and rollback). Returns total released.
    int releaseOrderReservations(const string& orderId);
    // Module 6 collection (uses rackCollect).
    bool collectStock(const string& productId,
                      const string& rackId, int qty,
                      const string& orderId);
    // Reserved quantity of an order on one rack (for the
    // Module 6 pre-check "stock still there?"). 0 if none.
    int reservedFor(const string& productId,
                    const string& rackId,
                    const string& orderId) const;
};

#endif
