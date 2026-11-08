#ifndef ROBOT_H
#define ROBOT_H
#include "common.h"

class Robot {
protected:
    string id;
    string locationId;
    double battery;        // 0..100
    double maxCapacity;    // kg
    RobotStatus status;
    string assignedTask;   // order id, "" = none
public:
    Robot* next;           // linked list link
    Robot(const string& id, const string& loc,
          double battery, double cap);
    virtual ~Robot() {}
    NO_COPY(Robot)

    // ---- pure virtual: each subclass answers differently ----
    virtual RobotType getType() const = 0;
    // 0.20 / 0.25 / 0.35 (use the RATE_ constants)
    virtual double batteryRate() const = 0;
    // Is maxCapacity inside this type's allowed range?
    //   Picker <=15, Carrier 15..30, Heavy <=60 (must be > 0).
    // Used when loading robots.txt.
    virtual bool isCapacityValid() const = 0;
    // Type eligibility rule from Module 4:
    //  Picker : heaviestUnit <= 5
    //  Carrier: always true
    //  Heavy  : totalWeight > 15 OR heaviestUnit > 10
    virtual bool isEligible(double totalWeight,
                            double heaviestUnit) const = 0;

    // ---- common behaviour ----
    // heaviest single unit must fit in the capacity
    bool canCarryUnit(double unitWeight) const;
    bool needsCharging() const;   // battery < 20 (this IS the flag)
    bool hasActiveTask() const;   // assignedTask != ""
    // Print one line. Append "[NEEDS CHARGING]" when
    // needsCharging() is true: this is how a robot is "flagged".
    void display() const;
    // getters / setters (trivial, write them all)
    string getId() const;
    string getLocation() const;
    double getBattery() const;
    double getCapacity() const;
    RobotStatus getStatus() const;
    string getTask() const;
    void setLocation(const string& loc);
    void setBattery(double b);        // clamp to 0..100
    void setStatus(RobotStatus s);
    void setTask(const string& orderId);
};

// Each subclass: constructor calls Robot's constructor and
// overrides the 4 pure virtuals.
class Picker : public Robot {
public:
    Picker(const string& id, const string& loc,
           double battery, double cap);
    RobotType getType() const override;
    double batteryRate() const override;
    bool isCapacityValid() const override;
    bool isEligible(double totalWeight,
                    double heaviestUnit) const override;
};

class Carrier : public Robot {
public:
    Carrier(const string& id, const string& loc,
            double battery, double cap);
    RobotType getType() const override;
    double batteryRate() const override;
    bool isCapacityValid() const override;
    bool isEligible(double totalWeight,
                    double heaviestUnit) const override;
};

class HeavyDuty : public Robot {
public:
    HeavyDuty(const string& id, const string& loc,
              double battery, double cap);
    RobotType getType() const override;
    double batteryRate() const override;
    bool isCapacityValid() const override;
    bool isEligible(double totalWeight,
                    double heaviestUnit) const override;
};

#endif
