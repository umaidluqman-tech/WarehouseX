#ifndef COMMON_H
#define COMMON_H
#include <string>
using std::string;

// Forbid accidental copies of any class that owns raw pointers
// (a copy would be deleted twice -> crash / leak). Put this line
// inside the class: NO_COPY(ClassName)
#define NO_COPY(T) T(const T&) = delete; T& operator=(const T&) = delete;

// ---- enums shared by all modules ----
enum LocationType { LOC_RACK, LOC_PACKING, LOC_CHARGING };
enum PathStatus   { PATH_OPEN, PATH_BLOCKED };
enum OrderStatus  { ORD_PENDING, ORD_RESERVED, ORD_ASSIGNED,
                    ORD_PARTIAL, ORD_COMPLETED, ORD_CANCELLED };
enum RobotType    { ROBOT_PICKER, ROBOT_CARRIER, ROBOT_HEAVY };
enum RobotStatus  { RS_AVAILABLE, RS_ASSIGNED, RS_WAIT_CHARGE,
                    RS_CHARGING, RS_UNAVAILABLE };

// ---- constants from "Global Rules" section 3 of the PDF ----
const double SAFETY_RESERVE = 10.0; // plan needs battery >= cost+10
const double LOW_BATTERY    = 20.0; // below this -> needs charging
const double RATE_PICKER    = 0.20;
const double RATE_CARRIER   = 0.25;
const double RATE_HEAVY     = 0.35;
const int    NO_ROUTE       = -1;   // "distance" when unreachable
const int    MAX_SLOTS      = 10;   // max slots in one station
const int    MAX_PARTS      = 8;    // max fields on one file line
const double EPS            = 1e-6; // tolerance for double compares

#endif
