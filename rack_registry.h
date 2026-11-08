#ifndef RACK_REGISTRY_H
#define RACK_REGISTRY_H
#include "common.h"
#include "graph.h"

// Maps RackID -> LocationID (each rack sits at exactly 1
// RACK-type location). Stored as a linked list.
struct RackLocNode {
    string rackId;
    string locationId;
    RackLocNode* next;
};

class RackRegistry {
private:
    RackLocNode* head;
    // delete all nodes (used by destructor)
    void clearAll();
public:
    RackRegistry();
    ~RackRegistry();
    NO_COPY(RackRegistry)
    // Read racks.txt. For each line check:
    //   rackId valid ("RK"+digits), unique,
    //   location exists in graph AND is type LOC_RACK.
    bool loadRacks(const string& fileName, const Graph& g);
    // Does this rack exist in racks.txt? (Module 2 uses it)
    bool rackExists(const string& rackId) const;
    // Location of a rack. false if rack unknown.
    bool getLocationOfRack(const string& rackId,
                           string& locationOut) const;
};

#endif
