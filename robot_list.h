#ifndef ROBOT_LIST_H
#define ROBOT_LIST_H
#include "robot.h"
#include "graph.h"

class RobotList {
private:
    Robot* head;
    int count;
    void clearAll();   // delete each robot (virtual dtor)
public:
    RobotList();
    ~RobotList();
    NO_COPY(RobotList)
    // 1. robots.txt: create Picker/Carrier/HeavyDuty by the
    //    type word; REJECT if isCapacityValid() is false, id
    //    not "RB"+digits, id duplicate, location missing,
    //    battery not 0..100. Print why for every rejected line.
    bool loadRobots(const string& file, const Graph& g);
    // 2. register new robot (unique id "RB"+digits, location
    //    exists, valid type/capacity)
    bool registerRobot(const string& id, RobotType type,
                       double battery, double cap,
                       const string& loc, const Graph& g);
    // 3. find (keep the name findRobot - Member 1 uses it)
    Robot* findRobot(const string& id) const;
    // 4 / 5. display
    void displayAll() const;
    void displayAvailable() const;
    // 6. battery update 0..100
    bool updateBattery(const string& id, double level);
    // 7. Assign robot to order. Reject if the robot is not
    //    Available or already has a task (rule 10).
    //    Sets status=RS_ASSIGNED and task=orderId.
    bool assignRobot(const string& id, const string& orderId);
    // 8. Release: task="", status=Available; if battery<20
    //    print "RBxx needs charging" (the robot also shows
    //    [NEEDS CHARGING] in every display).
    bool releaseRobot(const string& id);
    // 9. Maintenance on/off. Reject if the robot has an active
    //    task or is Charging. Return old status for undo.
    //    A robot that is Waiting for Charging is also rejected
    //    here: the MENU must first call
    //    ChargerManager::removeFromQueue (status -> Available)
    //    and then call this function.
    //    on=true : Available -> Unavailable
    //    on=false: Unavailable -> Available (reject otherwise)
    bool setMaintenance(const string& id, bool on,
                        RobotStatus& oldStatusOut);
    // for planning loops
    Robot* getHead() const { return head; }
};

#endif
