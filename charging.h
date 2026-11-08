#ifndef CHARGING_H
#define CHARGING_H
#include "common.h"
#include "graph.h"
#include "robot.h"        // Member 3
#include "robot_list.h"   // Member 3

// FIFO queue of robot IDs, singly linked with front + rear.
struct RQNode { string robotId; RQNode* next; };

class RobotQueue {
private:
    RQNode* front;
    RQNode* rear;
    int size;
public:
    RobotQueue();
    ~RobotQueue();                  // delete every node
    NO_COPY(RobotQueue)
    // add at rear (update rear; if empty also set front)
    void enqueue(const string& robotId);
    // remove from front, return its id. false if empty.
    bool dequeue(string& robotIdOut);
    // look at front without removing.
    bool peek(string& robotIdOut) const;
    // linear scan: is this robot waiting here?
    bool contains(const string& robotId) const;
    // remove robot from ANYWHERE in the queue (fix rear too:
    // if the last node is removed, rear = its predecessor)
    bool removeById(const string& robotId);
    int  getSize() const;
    bool isEmpty() const;
    // prints "Front -> RB01 -> RB03 -> NULL"
    void display() const;
};

class ChargingStation {
public:
    string id;                    // C01
    string locationId;            // L14
    int totalSlots;               // <= MAX_SLOTS
    string slots[MAX_SLOTS];      // robotId per slot, "" = free
    RobotQueue waiting;           // own FIFO queue
    ChargingStation* next;

    ChargingStation(const string& id, const string& loc,
                    int slots);
    NO_COPY(ChargingStation)
    int  freeSlotCount() const;   // count slots that are ""
    bool hasFreeSlot() const;
    // put robot in first free slot. false if none free.
    bool occupySlot(const string& robotId);
    // free the slot holding this robot. false if not found.
    bool freeSlotOf(const string& robotId);
};

class ChargerManager {
private:
    ChargingStation* head;
    ChargingStation* findStation(const string& id) const;
    // station that currently holds / queues this robot, or null
    ChargingStation* findStationOfRobot(
                         const string& robotId) const;
    // Start charging 'robot' on 'st' (slot must be free):
    //   occupySlot, status=RS_CHARGING, location=st->locationId
    void beginCharging(ChargingStation* st, Robot* robot);
    // Can this robot be charged / queued at all?
    //   status Available or Waiting for Charging (an active task,
    //   Charging or Unavailable is refused) AND battery < 100.
    //   Prints the reason when refused.
    bool isEligibleForCharging(const Robot* robot) const;
    void clearAll();
public:
    ChargerManager();
    ~ChargerManager();
    NO_COPY(ChargerManager)

    // Read chargers.txt (id "C"+digits, unique, station must be
    // on a LOC_CHARGING location, slots 1..MAX_SLOTS, and only
    // ONE station per location).
    bool loadChargers(const string& file, const Graph& g);

    // Op 1: show all stations: total/free slots, robots on
    // slots, waiting queue.
    void displayAll() const;
    // Op 2: print every robot with battery < 20 (the PDF
    // "needs charging" rule) that is Available or Waiting for
    // Charging.
    void listRobotsNeedingCharge(const RobotList& robots) const;
    // Op 3: nearest reachable station with a FREE slot; if no
    // station has a free slot, nearest reachable station
    // (its queue will be used). Ties -> compareIds smaller
    // station ID. Also check the robot has enough battery to
    // TRAVEL there (legConsumption with carried=0 and the
    // robot's rate; no 10% reserve needed).
    // false = cannot reach any station -> caller suggests
    // setting the robot Unavailable.
    // Hint: loop stations, use graph.shortestDistance.
    bool findStationForRobot(Graph& g, const Robot* robot,
                             ChargingStation*& stationOut,
                             bool& freeSlotOut);
    // Op 4: does that station have a free slot?
    bool isSlotAvailable(const string& stationId) const;
    // Op 5: join the station's queue; status = RS_WAIT_CHARGE.
    // Reject if the robot is not eligible (see
    // isEligibleForCharging) or is already queued/charging.
    bool addToQueue(const string& stationId, Robot* robot);
    // Op 6 (main entry): checks eligibility, picks the station
    // (Op 3), then starts charging or queues the robot.
    // ANY Available robot below 100% may be charged (so the
    // "suggest charging" advice of Module 5 always works);
    // below 20% is only the "needs charging" flag.
    bool requestCharging(Graph& g, Robot* robot);
    // Op 7: finish charge of this robot (must be Charging):
    //   battery=100, free the slot, status=Available, location
    //   stays at the station, THEN dequeue the next waiting
    //   robot of the SAME station and beginCharging it (find
    //   it with RobotList::findRobot).
    bool completeCharging(const string& robotId,
                          RobotList& robots);
    // Op 9: remove robot from the queue, status -> Available.
    bool removeFromQueue(const string& robotId,
                         RobotList& robots);
    // Op 10: "return a charged robot to Available". Safe version:
    //   Charging        -> calls completeCharging (never leaves a
    //                      slot occupied)
    //   Waiting         -> refuse: "use remove from queue"
    //   Unavailable     -> refuse: "use maintenance"
    //   already Available/Assigned -> refuse with a message
    bool returnToAvailable(const string& robotId,
                           RobotList& robots);
    // Op 8 is RobotList::updateBattery (call it from the menu;
    // refuse it for a robot that is Charging).
};

#endif
