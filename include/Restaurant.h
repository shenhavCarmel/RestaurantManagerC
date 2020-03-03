#ifndef RESTAURANT_H_
#define RESTAURANT_H_

#include <vector>
#include <string>
#include "Dish.h"
#include "Table.h"
#include "Action.h"
#include <sstream>
#include <fstream>
#include <iostream>

class Restaurant{
public:
    Restaurant();
    Restaurant(const std::string &configFilePath);
    void start();
    int getNumOfTables() const;
    Table* getTable(int ind);
    const std::vector<BaseAction*>& getActionsLog() const; // Return a reference to the history of actions
    std::vector<Dish>& getMenu();
    void close();
    void clear();
    bool isOpen();
    std::vector<Table*> getTables();

    // 1. Destructor
    ~Restaurant();

    // 2. Copy Constructor
    Restaurant(Restaurant &other);

    // 3. Move Constructor
    Restaurant(Restaurant &&other);

    // 4. Copy Assignment
    Restaurant& operator=(Restaurant &other);

    // 5. Move Assignment
    Restaurant& operator=(Restaurant &&other);

    int getCustomersCounter();
private:
    bool open;
    std::vector<Table*> tables;
    std::vector<Dish> menu;
    std::vector<BaseAction*> actionsLog;
    int customersCounter;
};

#endif