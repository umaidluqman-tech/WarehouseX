#ifndef GRAPH_H
#define GRAPH_H
#include "common.h"

// One pathway stored inside a location's adjacency list.
// An undirected edge A-B is stored TWICE (A's list and B's).
struct Edge {
    string toId;        // location at the other end
    int distance;       // must be > 0
    PathStatus status;  // OPEN or BLOCKED
    Edge* next;
};

// One graph node. Locations live in a singly linked list.
struct Location {
    string id;
    string name;
    LocationType type;
    Edge* edges;        // head of adjacency list
    Location* next;
    // ---- scratch fields for BFS / Dijkstra ----
    // Reset them at the start of every BFS / Dijkstra run.
    int dist;           // best distance so far (INT_MAX = unknown)
    int hops;           // number of locations on best route
    bool done;          // finalized (Dijkstra) / visited (BFS)
    Location* prev;     // previous node on best route
};

// One stop of a route. Routes are linked lists (no arrays).
struct RouteStop {
    string id;
    RouteStop* next;
};

// A route result: linked list of stops, start -> end.
// Whoever receives a Route* must delete it.
class Route {
private:
    RouteStop* head;
    RouteStop* tail;
public:
    int stopCount;       // number of stops (start and end included)
    int totalDistance;   // set by Graph
    Route();
    ~Route();            // delete every RouteStop
    NO_COPY(Route)
    void pushFront(const string& id);  // used while walking prev[]
    void pushBack(const string& id);
    const RouteStop* first() const { return head; }
    string startId() const;   // "" if empty
    string endId() const;     // "" if empty
};

class Graph {
private:
    Location* head;
    int locationCount;

    // ---------- private helpers ----------
    // Walk the list, return node with this id or nullptr.
    Location* findLocation(const string& id) const;
    // Walk loc->edges, return edge to toId or nullptr.
    Edge* findEdge(Location* loc, const string& toId) const;
    // Insert one directed edge at the front of loc's list.
    void addEdgeOneWay(Location* from, const string& toId, int d,
                       PathStatus st);
    // Delete the edge to toId from loc's list (fix pointers).
    // Returns false if not found.
    bool removeEdgeOneWay(Location* from, const string& toId);
    // Set status on BOTH directions of the pathway.
    bool setStatusBothWays(const string& a, const string& b,
                           PathStatus st);
    // Set dist=INT_MAX, hops=INT_MAX, done=false, prev=nullptr
    // for every location.
    void resetScratch();
    // Dijkstra step: among nodes with done==false and
    // dist!=INT_MAX, return the best one by
    //   1) smaller dist  2) smaller hops  3) compareIds(id)
    // Returns nullptr when none left.
    Location* pickClosestUnfinished();
    // Run Dijkstra from 'start' (stops early when 'target' is
    // finalized; target == nullptr means run to the end).
    void runDijkstra(Location* start, Location* target);
    // Free every Location and its Edge list (destructor).
    void clearAll();

public:
    Graph();
    ~Graph();   // call clearAll()
    NO_COPY(Graph)

    // ---------- loading ----------
    // Read locations.txt. ID must be "L"+digits, type valid,
    // ID unique. Skip bad/duplicate lines and print why.
    bool loadLocations(const string& fileName);
    // Read pathways.txt. Both ends must exist, a != b,
    // distance > 0, no duplicate pathway, status OPEN/BLOCKED.
    bool loadPathways(const string& fileName);

    // ---------- menu operations ----------
    // Add location: id "L"+digits, unique, type valid.
    bool addLocation(const string& id, const string& name,
                     LocationType type);
    // Add pathway: both exist, d > 0, not duplicate, a != b.
    bool addPathway(const string& a, const string& b, int d);
    // Menu option 1: delete the edge from BOTH adjacency lists.
    bool removePathway(const string& a, const string& b);
    // Menu option 2: keep edge, set BLOCKED (both directions).
    // Returns false (and prints why) if the pathway does not
    // exist OR is ALREADY blocked. So a successful block always
    // changed OPEN -> BLOCKED; Module 8 records that.
    bool blockPathway(const string& a, const string& b);
    // Set BLOCKED edge back to OPEN. false if not blocked.
    bool reopenPathway(const string& a, const string& b);
    // Print every location + its connections. Mark BLOCKED
    // pathways clearly, e.g. "L02(5) [BLOCKED]".
    void displayAll() const;
    // Search by id; print details. true if found.
    bool searchLocation(const string& id) const;

    // ---------- queries used by OTHER modules ----------
    bool locationExists(const string& id) const;
    // false if location missing.
    bool getLocationType(const string& id, LocationType& out) const;
    // Status of pathway a-b. false if no such pathway.
    bool getPathwayStatus(const string& a, const string& b,
                          PathStatus& out) const;

    // ---------- algorithms ----------
    // BFS using a small private linked-list queue (ignore
    // BLOCKED edges). True if 'to' can be reached from 'from'.
    // Return false if either id is missing (print which one).
    bool isConnected(const string& from, const string& to);

    // Dijkstra (ignore BLOCKED edges). Steps:
    //  1 resetScratch(); start.dist=0; start.hops=1
    //  2 loop: u = pickClosestUnfinished(); stop if null/target
    //  3 mark u done; for each OPEN edge u->v, v not done:
    //      nd = u.dist + edge.distance; nh = u.hops + 1
    //      if nd < v.dist OR (nd == v.dist AND nh < v.hops):
    //          v.dist=nd; v.hops=nh; v.prev=u
    //  4 if target.dist == INT_MAX -> return nullptr
    //  5 follow prev[] from the target and pushFront each id into
    //    a new Route; set totalDistance.
    // Tie-break (DOCUMENT THIS IN THE REPORT): equal distance ->
    // fewer locations; still equal -> the predecessor that was
    // finalized first is kept (nodes are finalized in the order
    // dist, hops, compareIds), so the result is deterministic.
    // from == to gives a 1-stop route with distance 0.
    // Returns nullptr if no route OR a location is missing: the
    // MENU must call locationExists() first so it can print
    // "start/end location does not exist" versus "no route".
    // CALLER DELETES the Route.
    Route* shortestRoute(const string& from, const string& to);

    // Same Dijkstra but only the number. NO_ROUTE if none.
    // (Delete the temporary Route inside: no leaks.)
    int shortestDistance(const string& from, const string& to);

    // Nearest location of a given type (PACKING or CHARGING)
    // by Dijkstra distance. Tie -> compareIds smaller id.
    // Used by Module 5 (packing) and Module 7 (charging).
    // Returns false if none reachable.
    bool findNearestOfType(const string& from, LocationType t,
                           string& outId, int& outDist);

    // true if EVERY pathway along the route is currently OPEN
    // (Module 6 validation). A 1-stop route is open.
    bool isRouteOpen(const Route* r) const;

    // Print "L01 -> L02 -> L06 -> L08 (total 11)".
    void displayRoute(const Route* r) const;
};

#endif
