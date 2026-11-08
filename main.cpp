// main.cpp - program entry point and top-level menu.
// Owner: team leader / Member 1 (agree in the team).
#include <iostream>
#include "graph.h"
#include "rack_registry.h"
#include "product_tree.h"
#include "robot_list.h"
#include "charging.h"
#include "order_system.h"
#include "plan_registry.h"
#include "plan_builder.h"
#include "executor.h"
#include "undo_stack.h"
#include "menus_m1.h"
#include "menus_m2.h"
#include "menus_m3.h"
#include "utils.h"
using std::cout;

int main() {
    // ---- all data structures live here; destructors free them ----
    Graph graph;
    RackRegistry racks;
    ProductTree products;
    RobotList robots;
    ChargerManager chargers;
    OrderSystem orders;
    PlanRegistry plans;
    UndoManager undo;
    PlanBuilder builder(graph, products, racks);
    Executor executor(graph, products, racks, orders, robots, plans);

    // ---- load everything from files (nothing is hardcoded) ----
    // The order matters: each file needs the one before it.
    cout << "Loading warehouse data...\n";
    if (!graph.loadLocations("locations.txt")) {
        cout << "locations.txt could not be loaded. Exiting.\n";
        return 1;
    }
    graph.loadPathways("pathways.txt");
    racks.loadRacks("racks.txt", graph);
    chargers.loadChargers("chargers.txt", graph);
    products.loadProducts("products.txt");
    products.loadInventory("inventory.txt", racks);
    robots.loadRobots("robots.txt", graph);
    orders.loadOrders("orders.txt", products);

    // ---- main menu (input validated, never crashes) ----
    bool running = true;
    while (running) {
        cout << "\n=====================================\n"
             << " INTELLIGENT WAREHOUSE SYSTEM\n"
             << "=====================================\n"
             << " 1. Layout and pathways (Module 1)\n"
             << " 2. Inventory and racks (Module 2)\n"
             << " 3. Customer orders (Module 3)\n"
             << " 4. Robot fleet (Module 4)\n"
             << " 5. Planning and execution (Modules 5, 6)\n"
             << " 6. Charging (Module 7)\n"
             << " 7. Undo history (Module 8)\n"
             << " 0. Exit\n";
        int choice = readIntInRange("Choose: ", 0, 7);
        switch (choice) {
            case 1: runGraphMenu(graph, undo); break;
            case 2: runInventoryMenu(products, racks, undo, orders); break;
            case 3: runOrderMenu(orders, products, robots, plans); break;
            case 4: runRobotMenu(robots, graph, undo, chargers); break;
            case 5: runPlanningMenu(builder, executor, orders,
                                    robots, plans); break;
            case 6: runChargingMenu(graph, chargers, robots); break;
            case 7: runUndoMenu(undo, products, graph, robots); break;
            case 0: running = false; break;
        }
    }
    cout << "Goodbye.\n";
    return 0;   // destructors run here and free every structure
}
