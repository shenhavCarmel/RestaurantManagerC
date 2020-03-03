#include "../include/Restaurant.h"
#include "../include/Dish.h"
#include "../include/Customer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

//
// Created by nofarcar@wincs.cs.bgu.ac.il on 11/11/18.
//


Restaurant::Restaurant(const std::string &configFilePath): open(false), tables{} , menu{}, actionsLog{}, customersCounter(0) {

    int numTables = 0;
    std::vector<string> tableSeats;
    std::vector<string> DishDetails;
    int dishCounter = 0;

    std:: string _lineRead;
    std::ifstream _file;
    _file.open(configFilePath);

    if (!_file.is_open())
        std::cout << "Error opening file";
    else {

        getline(_file,_lineRead);

        // number of tables
        while(_lineRead[0] == '#') {
            getline(_file, _lineRead);
        }
        numTables = std:: stoi(_lineRead);

        getline(_file,_lineRead);

        // number of seats in the tables
        while(_lineRead[0] == '#') {
            getline(_file, _lineRead);
        }

        boost::split(tableSeats, _lineRead, boost::is_any_of(","));

        // Init restaurant's tables
        for (int i=0; i< numTables; i++) {
            tables.push_back(new Table(std::stoi(tableSeats[i])));
        }

        // Dish details
        while(getline(_file,_lineRead) && _lineRead[0] == '#') {}

        int d_price;
        string d_name;
        DishType d_type;
        string d_typeString;

        // insert first dish
        boost::split(DishDetails, _lineRead, boost::is_any_of(","));
        d_name = DishDetails[0];
        d_typeString = DishDetails[1];
        d_price = stoi(DishDetails[2]);
        if (d_typeString == "VEG")
            d_type = VEG;
        else if (d_typeString == "SPC")
            d_type = SPC;
        else if (d_typeString == "BVG")
            d_type = BVG;
        else if (d_typeString == "ALC")
            d_type = ALC;

        menu.push_back(Dish(dishCounter, d_name, d_price, d_type));
        dishCounter++;

        while(getline(_file,_lineRead)) {
            boost::split(DishDetails, _lineRead, boost::is_any_of(","));

            d_name = DishDetails[0];
            d_typeString = DishDetails[1];

            if (d_typeString == "VEG")
                d_type = VEG;
            else if (d_typeString == "SPC")
                d_type = SPC;
            else if (d_typeString == "BVG")
                d_type = BVG;
            else if (d_typeString == "ALC")
                d_type = ALC;

            d_price = stoi(DishDetails[2]);

            menu.push_back(Dish(dishCounter, d_name, d_price, d_type));

            dishCounter++;
        }
        _file.close();
    }
}



// Copy constructor
Restaurant::Restaurant(Restaurant &other) : open(other.isOpen()), tables(), menu(), actionsLog(), customersCounter(other.getCustomersCounter()) {

    // copy actions log
    for (int i=0; i< static_cast<int>(other.getActionsLog().size()); i++) {
       actionsLog.push_back(other.actionsLog[i]->clone());
    }

    for (int i = 0; i < static_cast<int>(other.menu.size()); i++) {
        menu.emplace_back(Dish(other.menu[i].getId(), other.menu[i].getName(), other.menu[i].getPrice(),other.menu[i].getType()));
    }

    // copy tables
    for(Table* table: other.tables){
        tables.push_back(table->clone());
    }
}

// Copy assignment
Restaurant& Restaurant::operator=(Restaurant &other) {
    if (this != &other)
    {
        this->clear();

        open = other.isOpen();
        customersCounter = other.getCustomersCounter();

        for(Table* table: other.tables){
            tables.push_back(table->clone());
        }

        for(BaseAction* action:other.actionsLog){
            actionsLog.push_back(action->clone());
        }

    }
    return *this;
}

// Move Constructor
Restaurant::Restaurant(Restaurant &&other) : open(other.open) , tables(other.tables) , menu(other.menu) , actionsLog(other.getActionsLog()), customersCounter(other.customersCounter){
    this->clear();

    customersCounter = other.getCustomersCounter();
    menu.clear();
    for(Dish dish:other.menu){
        menu.push_back(dish);
    }

    other.tables.clear();
    other.menu.clear();
    other.actionsLog.clear();
}

// Move Assignment
Restaurant& Restaurant::operator=(Restaurant &&other) {

    if (this != &other) {
        this->clear();
        open = other.isOpen();
        menu.clear();
        for(Dish dish:other.menu){
            menu.push_back(dish);
        }

        tables = other.getTables();
        actionsLog = other.getActionsLog();
        other.tables.clear();
        other.actionsLog.clear();
        other.menu.clear();
    }
    return *this;
}

// Destructor
Restaurant::~Restaurant() { this->clear(); }


void Restaurant::clear() {
    for(BaseAction* act:actionsLog){
        delete act;
    }
    actionsLog.clear();

    for(Table* table:tables){
        delete table;
    }
    tables.clear();
}

int Restaurant::getCustomersCounter() {
    return customersCounter;
}
std::vector<Dish> & Restaurant::getMenu() {
    return menu;
}

const std::vector<BaseAction *> & Restaurant::getActionsLog() const {
    return actionsLog;
}

int Restaurant::getNumOfTables() const {
    return tables.size();
}

Table * Restaurant::getTable(int ind) {
    if (ind < 0 || (unsigned int)ind >= tables.size())
        return nullptr;
    return tables[ind];
}

void Restaurant::start() {
    std::cout << "Restaurant is now open!" << endl;

    open = true;

    std::string inputType;
    std::cin >> inputType;

    while (inputType != "closeall") {

        BaseAction *action;

        if (inputType == "open") {

            int openActionTableId;
            std::cin >> openActionTableId;

            std::vector<Customer *> tableCustomers;
            std::string customersInfo;
            getline(cin, customersInfo);

            std::istringstream FixCustomersInfo(customersInfo.c_str());
            string currCustomer;

            while(FixCustomersInfo >> currCustomer) {
                int spacePos = static_cast<int>(currCustomer.find(','));
                string name = currCustomer.substr(0,spacePos);
                string type = currCustomer.substr(spacePos+1,currCustomer.size());

                if (type == "chp") {
                    Customer *custCHP = new CheapCustomer(name, customersCounter);
                    tableCustomers.push_back(custCHP);
                    customersCounter++;
                } else if (type == "veg") {

                    Customer *custVEG = new VegetarianCustomer(name, customersCounter);
                    tableCustomers.push_back(custVEG);
                    customersCounter++;
                } else if (type == "alc") {
                    Customer *custALC = new AlchoholicCustomer(name, customersCounter);
                    tableCustomers.push_back(custALC);
                    customersCounter++;
                } else if (type == "spc") {
                    Customer *custSPC = new SpicyCustomer(name, customersCounter);
                    tableCustomers.push_back(custSPC);
                    customersCounter++;
                }
            }
            action = new OpenTable(openActionTableId, tableCustomers);

        }
        else if (inputType == "order") {
            int tableId;
            std::cin >> tableId;
            action = new Order(tableId);
        }
        else if (inputType == "move") {
            int src;
            std::cin >> src;
            int dst;
            std::cin >> dst;
            int id;
            std::cin >> id;
            action = new MoveCustomer(src, dst, id);
        }
        else if (inputType == "close") {
            int tableId;
            std::cin >> tableId;
            action = new Close(tableId);
        }
        else if (inputType == "status") {
            int tableID;
            std::cin >> tableID;
            action = new PrintTableStatus(tableID);
        }
        else if (inputType == "log") {
            action = new PrintActionsLog();
        }
        else if (inputType == "menu") {
            action = new PrintMenu();
        }
        else if (inputType == "backup") {
            action = new BackupRestaurant();
        }else if (inputType == "restore") {
            action = new RestoreResturant();
        }


        action->act(*this);
        this->actionsLog.push_back(action);

        std::cin >> inputType;
    }
    if (inputType == "closeall") {
        BaseAction *action;
        action = new CloseAll();
        action->act(*this);
        this->actionsLog.push_back(action);
    }


}

void Restaurant::close() {
   // std::cout << "Restaurant is now closed!" << std::endl;
    open = false;
}

bool Restaurant::isOpen() {
    return open;
}

std::vector<Table*> Restaurant::getTables() {
    return tables;
}