#ifndef UTILS_H
#define UTILS_H
#include "common.h"

// ---- string helpers ----
// Removes leading/trailing spaces, tabs, '\r'.
string trim(const string& s);
// Returns s in UPPER CASE (so "l01" and "L01" match).
string toUpperCase(const string& s);
// Splits 'line' at 'sep' into parts[] (max maxParts pieces).
// Returns number of pieces. Each piece is trimmed.
// Used by every file loader.
int splitLine(const string& line, char sep,
              string parts[], int maxParts);
// True if id = prefix + 1 or more digits. e.g. ("RK04","RK").
// "RK" alone is not valid, "RKX1" is not valid, and
// ("CU12","C") is NOT valid (after "C" only digits may follow).
bool isValidId(const string& id, const string& prefix);
// True if every char is a digit and s is not empty.
bool isPositiveInteger(const string& s);
// Converts string to int / double (return false if invalid).
bool parseInt(const string& s, int& out);
bool parseDouble(const string& s, double& out);

// ---- deterministic ID comparison (used by EVERY tie-break) ----
// Compares the letter prefix first, then the NUMBER part as a
// number, so "L9" < "L10" and "P200" < "P1000".
// Returns <0 if a<b, 0 if equal, >0 if a>b.
// Hint: strip leading zeros, compare digit-string LENGTH first,
// then the digits (no overflow for long numbers).
int compareIds(const string& a, const string& b);

// ---- enum <-> text ----
string locationTypeToString(LocationType t);
bool   stringToLocationType(const string& s, LocationType& out);
string orderStatusToString(OrderStatus s);
string robotStatusToString(RobotStatus s);
string robotTypeToString(RobotType t);
bool   stringToRobotType(const string& s, RobotType& out);

// ---- safe console input (never crash on bad input) ----
// Re-asks until the user types an int within [lo, hi].
// Hint: read with getline, then parseInt (do NOT use cin >> int).
int    readIntInRange(const string& prompt, int lo, int hi);
double readDoubleInRange(const string& prompt, double lo,
                         double hi);
// Reads a non-empty line (already trimmed + uppercased).
string readId(const string& prompt);
// Reads a non-empty line as is (names etc.).
string readText(const string& prompt);
// y/n question.
bool   readYesNo(const string& prompt);

// ---- battery formula (Global Rules section 3) ----
// consumption = distance * rate * (1 + 0.5*carried/maxCapacity)
// carried = 0 for an empty leg.
double legConsumption(int distance, double rate,
                      double carriedKg, double maxCapacityKg);

#endif
