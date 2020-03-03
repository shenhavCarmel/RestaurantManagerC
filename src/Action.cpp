#ifndef RESTSYS_ACTION_H_
#define RESTSYS_ACTION_H_

#include <iostream>
#include "../include/Action.h"
#include "../include/Customer.h"
#include "../include/Restaurant.h"
#include "../include/Table.h"
#include "../include/Dish.h"

using std::endl;

extern Restaurant* backup;


BaseAction::BaseAction() : errorMsg(), status(PENDING), fullStatus("Pending") {}

void BaseAction::complete() {
    this->status = COMPLETED;
    this->fullStatus = this->toString()  + " Completed";
}

void BaseAction::error(std::string errorMsg) {
    this->errorMsg = errorMsg;
    this->status = ERROR;
    this->fullStatus = this->toString() + " Error: " + errorMsg;
}

BaseAction::~BaseAction() {};

std::string BaseAction::getFullStatus() {
    return fullStatus;
}

std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}

void BaseAction::act(Restaurant &restaurant) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}


// -------------------- open table -------------------- //

OpenTable::OpenTable(int id, std::vector<Customer *> &customersList): tableId(id), customers(customersList){
}

OpenTable *OpenTable::clone() {
    return new OpenTable(*this);
}

// Copy constructor
OpenTable::OpenTable(const OpenTable &other) : tableId(other.tableId), customers() {
    for(Customer* cust: other.customers){
        customers.push_back(cust->clone());
    }

    if (other.getStatus() == ERROR) {
        error(other.getErrorMsg());
    } else {
        complete();
    }
}

// Move Constructor
OpenTable::OpenTable(OpenTable&& other) : tableId(other.tableId), customers() {
    this->customers = other.customers;
    other.customers.clear();
}

// Destructor
OpenTable::~OpenTable() {
    for (Customer* cust : customers) {
        delete cust;
    }
    customers.clear();
}

void OpenTable::act(Restaurant &restaurant) {

    bool completed = false;
    if (restaurant.getTable(tableId) == nullptr || restaurant.getTable(tableId)->isOpen()) {
        error("Table does not exist or is already open");
        std::cout << "Error: Table does not exist or is already open" << endl;
    }
    else if (customers.size() > (unsigned int)restaurant.getTable(tableId)->getCapacity()) {
        error("Too many customers for this table");
        std::cout << "Error: Too many customers for this table" << endl;
    }
    else {
        restaurant.getTable(tableId)->openTable();
        for (int i = 0; i <static_cast<int>(customers.size()); i++) {
            restaurant.getTable(tableId)->addCustomer(customers[i]->clone());
        }
        completed = true;
    }
    if(completed)
        complete();
}

std::string OpenTable::toString() const {
    std::string out("open " + std:: to_string(tableId));

    for (int i=0; i<static_cast<int>(customers.size()); i++) {
        out = out + " " + customers[i]->toString();
    }
    return out;

}

// -------------------- open table -------------------- //


// -------------------- order ------------------------- //

Order::Order(int id) : tableId(id) {}

Order *Order::clone() {
    return new Order(*this);
}

void Order::act(Restaurant &restaurant) {

    if (restaurant.getTable(tableId) == nullptr || !restaurant.getTable(tableId)->isOpen()) {
        error("Table does not exist or is already open");
        std::cout << "Error: " + getErrorMsg() << std::endl;
    }
    else {
        std::string dishName;
        std::string customerName;
        restaurant.getTable(tableId)->order(restaurant.getMenu());

        std::vector<OrderPair> tableOrders;

        for (int i=0; i< static_cast<int>(restaurant.getTable(tableId)->getCurrOrderList().size()); i++) {
            tableOrders.push_back(restaurant.getTable(tableId)->getCurrOrderList()[i]);
        }

        for (int i=0; i< static_cast<int>(tableOrders.size()); i++) {
            customerName = restaurant.getTable(tableId)->getCustomer(tableOrders[i].first)->getName();
            dishName = tableOrders[i].second.getName();
            std::cout << customerName << " ordered " << dishName << endl;
        }

        tableOrders.clear();

        complete();
    }
}

std::string Order::toString() const {
    return "order " + std::to_string(tableId);
}

// -------------------- order ------------------------- //



// -------------------- Move customer ----------------- //

MoveCustomer::MoveCustomer(int src, int dst, int customerId): srcTable(src), dstTable(dst), id(customerId) {}

MoveCustomer *MoveCustomer::clone() {
    return new MoveCustomer(*this);
}

void MoveCustomer::act(Restaurant &restaurant) {
    if (!(restaurant.getTable(srcTable)->isOpen()) || !(restaurant.getTable(dstTable)->isOpen()) ||
        (restaurant.getTable(srcTable) == nullptr) || (restaurant.getTable(dstTable) == nullptr) || (restaurant.getTable(srcTable)->getCustomer(id) == nullptr) ||
        (restaurant.getTable(dstTable)->getCustomers().size() == (unsigned int)restaurant.getTable(dstTable)->getCapacity())) {

        error("Cannot move customer");
        std::cout << "Error: " + getErrorMsg() << std::endl;
    }
    else {

        std::vector<OrderPair> srcNewOrderList;
        for (int i=0; i< static_cast<int>(restaurant.getTable(srcTable)->getOrders().size()); i++) {
            if (restaurant.getTable(srcTable)->getOrders()[i].first != id) {

                // take the dish that doesn't belong to the moving customer
                srcNewOrderList.push_back(restaurant.getTable(srcTable)->getOrders()[i]);
            }
            else {

                // insert the dish the moving customer ordered into destination table orderList
                restaurant.getTable(dstTable)->getOrders().push_back(restaurant.getTable(srcTable)->getOrders()[i]);
            }
        }

        // clear old order list of source table and insert the new vector
        restaurant.getTable(srcTable)->getOrders().clear();
        for (int i=0; i< static_cast<int>(srcNewOrderList.size()); i++) {
            restaurant.getTable(srcTable)->getOrders().push_back(srcNewOrderList[i]);
        }
        srcNewOrderList.clear();

        Customer* customer = restaurant.getTable(srcTable)->getCustomer(id);
        restaurant.getTable(dstTable)->addCustomer(customer);

        restaurant.getTable(srcTable)->removeCustomer(id);

        if (restaurant.getTable(srcTable)->getCustomers().size() == 0) {
            restaurant.getTable(srcTable)->closeTable();
        }
        complete();
    }
}

std::string MoveCustomer::toString() const {
    return "move " + std::to_string(srcTable) + " " + std::to_string(dstTable) + " " + std::to_string(id);
}

// -------------------- Move customer ----------------- //


// -------------------- Close table ------------------- //

Close::Close(int id): tableId(id) {}

Close *Close::clone() {
    return new Close(*this);
}

void Close::act(Restaurant &restaurant) {

    if (restaurant.getTable(tableId) == nullptr || !(restaurant.getTable(tableId)->isOpen())) {
        error("Table does not exist or is not open");
        std::cout << "Error: " + getErrorMsg() << std::endl;
    }
    else {
        //std::cout <<"Bill: " <<  restaurant.getTable(tableId)->getBill() << std::endl;
        std::cout <<"Bill " <<  restaurant.getTable(tableId)->getBill() << std::endl;
        restaurant.getTable(tableId)->closeTable();
        complete();
    }
}

std::string Close::toString() const {
    return "close " + std::to_string(tableId);
}

// -------------------- Close table ------------------- //


// -------------------- Close all  -------------------- //

CloseAll::CloseAll() {}

CloseAll *CloseAll::clone() {
    return new CloseAll(*this);
}

void CloseAll::act(Restaurant &restaurant) {
    // TODO: check if the tables id is from zero and on..
    for(int i=0; i< static_cast<int >(restaurant.getNumOfTables()); i++){
        if (restaurant.getTable(i)->isOpen()) {
            //std::cout << "Table " << i << " was closed. Bill: " << restaurant.getTable(i)->getBill() << "NIS" << std::endl;
            std::cout << "Table " << i << " was closed. Bill " << restaurant.getTable(i)->getBill() << "NIS" << std::endl;
            restaurant.getTable(i)->closeTable();
        }
    }
    restaurant.close();
    complete();
}

std::string CloseAll::toString() const {
    return "close all";
}

// -------------------- Close all  -------------------- //



// -------------------- Print menu -------------------- //

PrintMenu::PrintMenu() {}

PrintMenu *PrintMenu::clone() {
    return new PrintMenu(*this);
}

void PrintMenu::act(Restaurant &restaurant) {
    for (int i=0; i< static_cast<int >(restaurant.getMenu().size()); i++){
        std::cout << restaurant.getMenu()[i].getName() << " " << restaurant.getMenu()[i].typeText() << " " << restaurant.getMenu()[i].getPrice() << "NIS" << std::endl;
    }
    complete();
}

std::string PrintMenu::toString() const {
    return "menu";
}

// -------------------- Print menu -------------------- //


// -------------------- Print table status ------------ //

PrintTableStatus::PrintTableStatus(int id): tableId(id) {}

PrintTableStatus *PrintTableStatus::clone() {
    return new PrintTableStatus(*this);
}

void PrintTableStatus::act(Restaurant &restaurant) {
    if (restaurant.getTable(tableId)->isOpen()){
        std::cout << "Table " << tableId << " status: open" << std::endl;
        std::cout << "Customers: " << std::endl;

        for (int i=0; i< static_cast<int >(restaurant.getTable(tableId)->getCustomers().size()); i++){
            std::cout << restaurant.getTable(tableId)->getCustomers()[i]->getId() << " " << restaurant.getTable(tableId)->getCustomers()[i]->getName() << std::endl;
        }
        std::cout << "Orders:" << std::endl;

        if (restaurant.getTable(tableId)->getOrders().size() != 0 ) {
            for (int i=0; i< static_cast<int >(restaurant.getTable(tableId)->getOrders().size()); i++) {
                std::cout << restaurant.getTable(tableId)->getOrders()[i].second.toString() << " " << std::to_string(restaurant.getTable(tableId)->getOrders()[i].first) << std::endl;
            }
        }

        std::cout << "Current Bill: " << restaurant.getTable(tableId)->getBill() << "NIS" << std::endl;
    }
    else{
        std::cout << "Table " << tableId << " status: closed" << std::endl;
    }
    complete();
}

std::string PrintTableStatus::toString() const {
    return "status " + std::to_string(tableId);
}

// -------------------- Print table status ------------ //


// -------------------- Print action log ------------- //

PrintActionsLog::PrintActionsLog() {}

PrintActionsLog *PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}

void PrintActionsLog::act(Restaurant &restaurant) {
    for (int i=0; i< static_cast<int>(restaurant.getActionsLog().size()); i++) {
        std::cout << restaurant.getActionsLog()[i]->getFullStatus() << endl;
    }
    complete();
}

std::string PrintActionsLog::toString() const {
    return "log";
}

// -------------------- Print action log ------------- //


// -------------------- Backup restaurant ------------ //

BackupRestaurant::BackupRestaurant() {}

BackupRestaurant *BackupRestaurant::clone() {
    return new BackupRestaurant(*this);
}

void BackupRestaurant::act(Restaurant &restaurant) {
    if (backup == nullptr) {
        backup = new Restaurant(restaurant);
    }
    else {
        *backup = restaurant;
    }
    complete();
}

std::string BackupRestaurant::toString() const {
    return "backup";
}

// -------------------- Backup restaurant ------------ //



// -------------------- Restore restaurant ----------- //

RestoreResturant::RestoreResturant() {}

RestoreResturant *RestoreResturant::clone() {
    return new RestoreResturant(*this);
}

void RestoreResturant::act(Restaurant &restaurant) {
    if (backup == nullptr) {
        error("Backup isn't available");
        std::cout << "Error: " + getErrorMsg() << std::endl;
    }
    else {
        restaurant = *backup;
        complete();
    }
}

std::string RestoreResturant::toString() const {
    return "restore";
}

// -------------------- Restore restaurant ----------- //


#endif